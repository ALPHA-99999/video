#include "udpreceiver.h"

#include <QDateTime>
#include <QDebug>
#include <QNetworkDatagram>

#include <algorithm>

namespace {
constexpr quint32 kStreamMagic = 0x4C425331; // "LBS1"
constexpr quint8 kStreamVersion = 1;
constexpr quint8 kStreamFlagKeyframe = 1u << 0;
constexpr quint8 kStreamFlagEndOfStream = 1u << 1;
constexpr int kStreamHeaderSize = 16;
constexpr quint16 kInvalidSyncOffset = 0xFFFF;
}

UdpReceiver::UdpReceiver(QObject *parent)
    : QObject(parent)
{
    m_statsTimer = new QTimer(this);
    connect(m_statsTimer, &QTimer::timeout, this, &UdpReceiver::updateStatistics);
    m_statsWindowTimer.start();
}

UdpReceiver::~UdpReceiver()
{
    stopListening();
}

bool UdpReceiver::startListening()
{
    if (m_udpSocket) {
        return true;
    }

    m_udpSocket = new QUdpSocket(this);
    if (!m_udpSocket->bind(QHostAddress::Any, m_port, QUdpSocket::ShareAddress)) {
        emit errorOccurred(QString("无法绑定到端口 %1: %2")
                               .arg(m_port)
                               .arg(m_udpSocket->errorString()));
        delete m_udpSocket;
        m_udpSocket = nullptr;
        return false;
    }

    m_udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 3080 * 2160 * 10);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::readPendingDatagrams);

    resetStreamState();
    m_statsTimer->start(1000);
    qDebug() << "开始监听UDP端口:" << m_port;
    return true;
}

void UdpReceiver::stopListening()
{
    if (m_udpSocket) {
        m_udpSocket->close();
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
    }

    if (m_statsTimer && m_statsTimer->isActive()) {
        m_statsTimer->stop();
    }

    resetStreamState();
}

StreamPacketHeader UdpReceiver::parseHeader(const QByteArray &data) const
{
    StreamPacketHeader header;
    if (data.size() < kStreamHeaderSize) {
        return header;
    }

    const quint8 *ptr = reinterpret_cast<const quint8 *>(data.constData());
    header.magic = (static_cast<quint32>(ptr[0]) << 24)
                 | (static_cast<quint32>(ptr[1]) << 16)
                 | (static_cast<quint32>(ptr[2]) << 8)
                 | static_cast<quint32>(ptr[3]);
    header.version = ptr[4];
    header.flags = ptr[5];
    header.headerSize = (static_cast<quint16>(ptr[6]) << 8) | static_cast<quint16>(ptr[7]);
    header.packetSeq = (static_cast<quint32>(ptr[8]) << 24)
                     | (static_cast<quint32>(ptr[9]) << 16)
                     | (static_cast<quint32>(ptr[10]) << 8)
                     | static_cast<quint32>(ptr[11]);
    header.payloadLen = (static_cast<quint16>(ptr[12]) << 8) | static_cast<quint16>(ptr[13]);
    header.syncOffset = (static_cast<quint16>(ptr[14]) << 8) | static_cast<quint16>(ptr[15]);
    return header;
}

void UdpReceiver::readPendingDatagrams()
{
    if (!m_udpSocket) {
        return;
    }

    while (m_udpSocket->hasPendingDatagrams()) {
        const qint64 recvMs = QDateTime::currentMSecsSinceEpoch();
        const QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        const QByteArray data = datagram.data();

        if (data.size() < kStreamHeaderSize) {
            continue;
        }

        const StreamPacketHeader header = parseHeader(data);
        if (header.magic != kStreamMagic) {
            qWarning() << "UDP packet magic invalid:" << QString::number(header.magic, 16);
            continue;
        }
        if (header.version != kStreamVersion) {
            qWarning() << "UDP packet version invalid:" << header.version;
            continue;
        }
        if (header.headerSize != kStreamHeaderSize) {
            qWarning() << "UDP packet header size invalid:" << header.headerSize;
            continue;
        }
        if (header.payloadLen > static_cast<quint16>(data.size() - header.headerSize)) {
            qWarning() << "UDP payload length invalid:" << header.payloadLen
                       << "datagramBytes=" << data.size();
            continue;
        }

        const QByteArray payload = data.mid(header.headerSize, header.payloadLen);
        processPacket(header, payload, recvMs);
    }
}

void UdpReceiver::processPacket(const StreamPacketHeader &header, const QByteArray &payload, qint64 recvMs)
{
    ++m_totalPackets;
    ++m_packetsSinceLog;

    if (!m_hasExpectedPacketSeq) {
        m_hasExpectedPacketSeq = true;
        m_expectedPacketSeq = header.packetSeq;
        m_waitingForKeyframe = true;
    }

    if (header.packetSeq > m_expectedPacketSeq) {
        const quint64 lost = static_cast<quint64>(header.packetSeq - m_expectedPacketSeq);
        m_lostPackets += lost;
        m_waitingForKeyframe = true;
        qWarning().noquote() << QString("UDP packet loss detected: expected=0x%1 got=0x%2 lost=%3")
                                    .arg(m_expectedPacketSeq, 8, 16, QLatin1Char('0')).toUpper()
                                    .arg(header.packetSeq, 8, 16, QLatin1Char('0')).toUpper()
                                    .arg(lost);
    } else if (header.packetSeq < m_expectedPacketSeq) {
        qDebug().noquote() << QString("UDP duplicate/out-of-order packet ignored: seq=0x%1 expected=0x%2")
                                  .arg(header.packetSeq, 8, 16, QLatin1Char('0')).toUpper()
                                  .arg(m_expectedPacketSeq, 8, 16, QLatin1Char('0')).toUpper();
        return;
    }

    const bool isKeyframePacket = (header.flags & kStreamFlagKeyframe) != 0;
    const bool isEndOfStream = (header.flags & kStreamFlagEndOfStream) != 0;

    if (m_waitingForKeyframe) {
        // 丢包或刚启动后，先等到新的 keyframe 同步点再把数据交给解码器。
        if (!isKeyframePacket) {
            m_expectedPacketSeq = header.packetSeq + 1;
            return;
        }

        const quint16 syncOffset = (header.syncOffset == kInvalidSyncOffset) ? 0 : header.syncOffset;
        if (syncOffset >= payload.size()) {
            qWarning() << "UDP keyframe sync offset out of range:"
                       << "seq=" << header.packetSeq
                       << "syncOffset=" << syncOffset
                       << "payloadSize=" << payload.size();
            m_expectedPacketSeq = header.packetSeq + 1;
            return;
        }

        // sync_offset 之前的字节属于旧同步点，丢失后不能再用于重建码流。
        const QByteArray syncedPayload = payload.mid(syncOffset);
        if (!syncedPayload.isEmpty()) {
            emitStreamChunk(syncedPayload, recvMs, recvMs);
            ++m_totalFrames;
            ++m_framesSinceLog;
        }

        m_waitingForKeyframe = false;
        m_expectedPacketSeq = header.packetSeq + 1;
    } else {
        if (!payload.isEmpty()) {
            emitStreamChunk(payload, recvMs, recvMs);
            ++m_totalFrames;
            ++m_framesSinceLog;
        }
        m_expectedPacketSeq = header.packetSeq + 1;
    }

    if (isEndOfStream) {
        qDebug() << "UDP end-of-stream packet received, waiting for next keyframe to resume";
        m_waitingForKeyframe = true;
    }
}

void UdpReceiver::emitStreamChunk(const QByteArray &payload, qint64 firstRecvMs, qint64 assembledMs)
{
    emit frame_toproess(payload, firstRecvMs, assembledMs);
}

void UdpReceiver::resetStreamState()
{
    m_hasExpectedPacketSeq = false;
    m_expectedPacketSeq = 0;
    m_waitingForKeyframe = true;
}

void UdpReceiver::updateStatistics()
{
    const qint64 elapsedMs = m_statsWindowTimer.elapsed();
    if (elapsedMs < 5000) {
        return;
    }

    const double seconds = static_cast<double>(elapsedMs) / 1000.0;
    const double packetRate = static_cast<double>(m_packetsSinceLog) / seconds;
    const double chunkRate = static_cast<double>(m_framesSinceLog) / seconds;
    const double packetLossRate = (m_totalPackets == 0)
        ? 0.0
        : (static_cast<double>(m_lostPackets) * 100.0 / static_cast<double>(m_totalPackets + m_lostPackets));
    m_packetLossRate = packetLossRate;

    qDebug().noquote() << QString("UDP stats: packets=%1 chunks=%2 lost=%3 lossRate=%4%% packetRate=%5/s chunkRate=%6/s")
                              .arg(m_totalPackets)
                              .arg(m_totalFrames)
                              .arg(m_lostPackets)
                              .arg(packetLossRate, 0, 'f', 2)
                              .arg(packetRate, 0, 'f', 2)
                              .arg(chunkRate, 0, 'f', 2);

    m_packetsSinceLog = 0;
    m_framesSinceLog = 0;
    m_statsWindowTimer.restart();
}
