#include "mqttframereceiver.h"

#include <QDateTime>
#include <QDebug>
#include <algorithm>

namespace {
constexpr int kHeaderSize = 8;

bool frameContainsParameterSets(const QByteArray &frameData)
{
    const quint8 *data = reinterpret_cast<const quint8 *>(frameData.constData());
    const int size = frameData.size();
    bool hasVps = false;
    bool hasSps = false;
    bool hasPps = false;

    for (int i = 0; i + 5 < size; ++i) {
        int startCodeSize = 0;
        if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x01) {
            startCodeSize = 3;
        } else if (i + 4 < size &&
                   data[i] == 0x00 && data[i + 1] == 0x00 &&
                   data[i + 2] == 0x00 && data[i + 3] == 0x01) {
            startCodeSize = 4;
        }

        if (startCodeSize == 0) {
            continue;
        }

        const int nalHeaderIndex = i + startCodeSize;
        if (nalHeaderIndex >= size) {
            continue;
        }

        const quint8 nalUnitType = (data[nalHeaderIndex] >> 1) & 0x3F;
        if (nalUnitType == 32) {
            hasVps = true;
        } else if (nalUnitType == 33) {
            hasSps = true;
        } else if (nalUnitType == 34) {
            hasPps = true;
        }

        if (hasVps && hasSps && hasPps) {
            return true;
        }
    }

    return false;
}
}

MqttFrameReceiver::MqttFrameReceiver(QObject *parent)
    : QObject(parent)
{
}

void MqttFrameReceiver::ingestPacket(const QByteArray &packet, qint64 recvMs)
{
    if (packet.size() <= kHeaderSize) {
        return;
    }

    const MqttFrameHeader header = parseHeader(packet);
    if (header.frameSize == 0 || header.frameSize > kMaxFrameSize) {
        qWarning() << "MQTT frame size invalid:" << header.frameSize;
        return;
    }

    QList<quint16> staleFrames;
    for (auto it = m_frameBuffers.constBegin(); it != m_frameBuffers.constEnd(); ++it) {
        if (it.key() != header.frameId) {
            staleFrames.push_back(it.key());
        }
    }
    for (quint16 staleFrameId : staleFrames) {
        const quint32 expectedSize = m_frameSizes.value(staleFrameId, 0);
        qDebug().noquote() << QString("MQTT dropped incomplete frameId=0x%1 size=%2 on next frame arrival")
                                  .arg(staleFrameId, 4, 16, QLatin1Char('0')).toUpper()
                                  .arg(expectedSize);
        m_frameBuffers.remove(staleFrameId);
        m_frameSizes.remove(staleFrameId);
        m_frameFirstRecvMs.remove(staleFrameId);
    }

    qDebug().noquote() << QString("MQTT fragment frameId=0x%1 fragmentId=%2 frameSize=%3 packetBytes=%4")
                              .arg(header.frameId, 4, 16, QLatin1Char('0')).toUpper()
                              .arg(header.fragmentId)
                              .arg(header.frameSize)
                              .arg(packet.size());

    const int maxPayloadSize = packet.size() - kHeaderSize;
    const quint32 fragmentOffset = static_cast<quint32>(header.fragmentId) * static_cast<quint32>(maxPayloadSize);
    if (fragmentOffset >= header.frameSize) {
        qWarning() << "MQTT fragment offset out of range:"
                   << "frameId" << header.frameId
                   << "fragmentId" << header.fragmentId
                   << "offset" << fragmentOffset
                   << "frameSize" << header.frameSize;
        return;
    }

    const quint32 remainingBytes = header.frameSize - fragmentOffset;
    const int actualPayloadSize = std::min(static_cast<quint32>(maxPayloadSize), remainingBytes);
    QByteArray fragmentData = packet.mid(kHeaderSize, actualPayloadSize);
    m_frameBuffers[header.frameId][header.fragmentId] = fragmentData;
    if (!m_frameSizes.contains(header.frameId)) {
        m_frameSizes[header.frameId] = header.frameSize;
    }
    if (!m_frameFirstRecvMs.contains(header.frameId)) {
        m_frameFirstRecvMs[header.frameId] = recvMs;
    }

    processFrameData(header.frameId);
}

MqttFrameHeader MqttFrameReceiver::parseHeader(const QByteArray &data) const
{
    MqttFrameHeader header;
    if (data.size() < kHeaderSize) {
        return header;
    }

    const quint8 *ptr = reinterpret_cast<const quint8 *>(data.constData());
    header.frameId = (ptr[0] << 8) | ptr[1];
    header.fragmentId = (ptr[2] << 8) | ptr[3];
    header.frameSize = (static_cast<quint32>(ptr[4]) << 24)
                     | (static_cast<quint32>(ptr[5]) << 16)
                     | (static_cast<quint32>(ptr[6]) << 8)
                     | static_cast<quint32>(ptr[7]);
    return header;
}

void MqttFrameReceiver::processFrameData(quint16 frameId)
{
    if (!m_frameBuffers.contains(frameId) || !m_frameSizes.contains(frameId)) {
        return;
    }

    QMap<quint16, QByteArray> &fragments = m_frameBuffers[frameId];
    const quint32 expectedSize = m_frameSizes[frameId];

    quint32 receivedSize = 0;
    for (const QByteArray &fragment : fragments) {
        receivedSize += static_cast<quint32>(fragment.size());
    }

    if (receivedSize < expectedSize) {
        return;
    }

    QByteArray completeFrame;
    completeFrame.reserve(static_cast<int>(expectedSize));
    QList<quint16> fragmentIds = fragments.keys();
    std::sort(fragmentIds.begin(), fragmentIds.end());

    for (quint16 fragmentId : fragmentIds) {
        completeFrame.append(fragments[fragmentId]);
        if (completeFrame.size() >= static_cast<int>(expectedSize)) {
            completeFrame.resize(static_cast<int>(expectedSize));
            break;
        }
    }

    if (completeFrame.size() != static_cast<int>(expectedSize)) {
        return;
    }

    const qint64 firstRecvMs = m_frameFirstRecvMs.value(frameId, QDateTime::currentMSecsSinceEpoch());
    const qint64 assembledMs = QDateTime::currentMSecsSinceEpoch();
    const bool hasParameterSets = frameContainsParameterSets(completeFrame);
    if (hasParameterSets) {
        qDebug().noquote() << QString("MQTT assembled frameId=0x%1 size=%2 fragments=%3 contains VPS/SPS/PPS")
                                  .arg(frameId, 4, 16, QLatin1Char('0')).toUpper()
                                  .arg(expectedSize)
                                  .arg(fragmentIds.size());
    } else {
        qDebug().noquote() << QString("MQTT assembled frameId=0x%1 size=%2 fragments=%3")
                                  .arg(frameId, 4, 16, QLatin1Char('0')).toUpper()
                                  .arg(expectedSize)
                                  .arg(fragmentIds.size());
    }
    emit frame_toproess(completeFrame, firstRecvMs, assembledMs);

    m_frameBuffers.remove(frameId);
    m_frameSizes.remove(frameId);
    m_frameFirstRecvMs.remove(frameId);
}

void MqttFrameReceiver::cleanupExpiredFrames(qint64 nowMs)
{
    QList<quint16> expiredFrames;
    for (auto it = m_frameFirstRecvMs.constBegin(); it != m_frameFirstRecvMs.constEnd(); ++it) {
        if (nowMs - it.value() > kFrameTimeoutMs) {
            expiredFrames.push_back(it.key());
        }
    }

    for (quint16 frameId : expiredFrames) {
        const quint32 expectedSize = m_frameSizes.value(frameId, 0);
        qDebug().noquote() << QString("MQTT dropped incomplete frameId=0x%1 size=%2")
                                  .arg(frameId, 4, 16, QLatin1Char('0')).toUpper()
                                  .arg(expectedSize);
        m_frameBuffers.remove(frameId);
        m_frameSizes.remove(frameId);
        m_frameFirstRecvMs.remove(frameId);
    }
}
