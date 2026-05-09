#include "udpreceiver.h"
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <QThread>
#include <algorithm>
UdpReceiver::UdpReceiver(QObject *parent)
    : QObject(parent)
    , m_udpSocket(nullptr)
    , m_port(3334)
    , m_totalPackets(0)
    , m_totalFrames(0)
    , m_lostPackets(0)
    , m_lastFrameId(0)
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

    m_udpSocket = new QUdpSocket(this);

    if (!m_udpSocket->bind(QHostAddress::Any, m_port, QUdpSocket::ShareAddress)) {
        emit errorOccurred(QString("无法绑定到端口 %1: %2")
                               .arg(m_port)
                               .arg(m_udpSocket->errorString()));
        delete m_udpSocket;
        m_udpSocket = nullptr;
        return false;
    }

    // 设置接收缓冲区大小（可根据需要调整）
    m_udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 3080 * 2160 * 10); // 10MB

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::readPendingDatagrams);
    //connect(this, &UdpReceiver::frameReceived, this, &UdpReceiver::Proessframe);
    m_statsTimer->start(1000); // 每秒更新一次统计信息

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

    if (m_statsTimer->isActive()) {
        m_statsTimer->stop();
    }

    QMutexLocker locker(&m_mutex);
    m_frameBuffers.clear();
    m_frameSizes.clear();
    m_frameFirstRecvMs.clear();
}

FrameHeader UdpReceiver::parseHeader(const QByteArray &data)
{
    FrameHeader header;

    if (data.size() < 8) {
        return header;
    }
    // 解析头部数据（注意字节序，假设为网络字节序）
    const quint8 *ptr = reinterpret_cast<const quint8*>(data.constData());

    header.frameId = (ptr[0] << 8) | ptr[1];
    header.fragmentId = (ptr[2] << 8) | ptr[3];
    header.frameSize = (ptr[4] << 24) | (ptr[5] << 16) | (ptr[6] << 8) | ptr[7];

    return header;
}

void UdpReceiver::readPendingDatagrams()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        const qint64 recvMs = QDateTime::currentMSecsSinceEpoch();
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QByteArray data = datagram.data();

        if (data.size() <= 8) {
            continue; // 数据太短，忽略
        }

        m_totalPackets++;
        ++m_packetsSinceLog;

        // 解析头部
        FrameHeader header = parseHeader(data);

        if (header.frameSize == 0 || header.frameSize > 10 * 1024 * 1024) {
            // 帧大小异常，忽略
            continue;
        }

        QMutexLocker locker(&m_mutex);

        // 存储分片数据（去掉8字节头部）
        QByteArray fragmentData = data.mid(8);
        m_frameBuffers[header.frameId][header.fragmentId] = fragmentData;

        // 记录帧的总大小
        if (!m_frameSizes.contains(header.frameId)) {
            m_frameSizes[header.frameId] = header.frameSize;
        }
        // 记录该帧第一个分片到达时间（用于端到端延迟统计）
        if (!m_frameFirstRecvMs.contains(header.frameId)) {
            m_frameFirstRecvMs[header.frameId] = recvMs;
        }

        // 检查是否收到完整帧
        processFrameData(header.frameId);

        // 清理旧帧（防止内存泄漏）
        if (m_frameBuffers.size() > 50) {
            cleanupOldFrames();
        }
    }

}

void UdpReceiver::processFrameData(quint16 frameId)
{
    if (!m_frameBuffers.contains(frameId) || !m_frameSizes.contains(frameId)) {
        return;
    }

    QMap<quint16, QByteArray> &fragments = m_frameBuffers[frameId];
    quint32 expectedSize = m_frameSizes[frameId];

    // 计算已收到的数据大小
    quint32 receivedSize = 0;
    for (const QByteArray &fragment : fragments) {
        receivedSize += fragment.size();
    }

    // 如果收到的数据达到预期大小，则认为帧完整
    if (receivedSize == expectedSize) {
        // 组装完整帧
        // qDebug()<<receivedSize<<""<<expectedSize;
        QByteArray completeFrame;
        completeFrame.reserve(expectedSize);

        // 按照分片序号排序组装
        QList<quint16> fragmentIds = fragments.keys();
        std::sort(fragmentIds.begin(), fragmentIds.end());

        for (quint16 fragId : fragmentIds) {
            completeFrame.append(fragments[fragId]);

            // 如果已经达到预期大小，停止添加（可能有重复包）
            if (completeFrame.size() >= static_cast<int>(expectedSize)) {
                completeFrame.resize(expectedSize);
                break;
            }
        }

        if (completeFrame.size() == static_cast<int>(expectedSize)) {
            const qint64 firstRecvMs = m_frameFirstRecvMs.value(frameId, QDateTime::currentMSecsSinceEpoch());
            const qint64 assembledMs = QDateTime::currentMSecsSinceEpoch();
            emit frame_toproess(completeFrame, firstRecvMs, assembledMs);
            m_totalFrames++;
            ++m_framesSinceLog;

            // 移除已处理的帧
            m_frameBuffers.remove(frameId);
            m_frameSizes.remove(frameId);
            m_frameFirstRecvMs.remove(frameId);
        }
    }
}

void UdpReceiver::cleanupOldFrames()
{
    // 移除最旧的帧
    QList<quint16> frameIds = m_frameBuffers.keys();
    if (!frameIds.isEmpty()) {
        std::sort(frameIds.begin(), frameIds.end());
        int framesToRemove = qMin(10, frameIds.size() / 2);

        for (int i = 0; i < framesToRemove; i++) {
            m_frameBuffers.remove(frameIds[i]);
            m_frameSizes.remove(frameIds[i]);
            m_frameFirstRecvMs.remove(frameIds[i]);
        }
    }
}

void UdpReceiver::updateStatistics()
{
    const qint64 elapsedMs = m_statsWindowTimer.elapsed();
    if (elapsedMs < 5000) {
        return;
    }

    const double seconds = static_cast<double>(elapsedMs) / 1000.0;
    const double packetRate = static_cast<double>(m_packetsSinceLog) / seconds;
    const double frameRate = static_cast<double>(m_framesSinceLog) / seconds;

    qDebug().noquote() << QString("UDP stats: packets=%1 frames=%2 packetRate=%3/s frameRate=%4/s")
                              .arg(m_totalPackets)
                              .arg(m_totalFrames)
                              .arg(packetRate, 0, 'f', 2)
                              .arg(frameRate, 0, 'f', 2);

    m_packetsSinceLog = 0;
    m_framesSinceLog = 0;
    m_statsWindowTimer.restart();

   //  emit statsUpdated(m_totalPackets, m_totalFrames, m_packetLossRate);
}

// QImage UdpReceiver::frameDecoded(QImage image)
// {
//     m_displayLabel->setPixmap(QPixmap::fromImage(image)
//                                       .scaled(m_displayLabel->size(),
//                                               Qt::KeepAspectRatio,
//                                               Qt::SmoothTransformation));

//         m_displayLabel->setAlignment(Qt::AlignCenter);

// }
