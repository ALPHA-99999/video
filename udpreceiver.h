#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <QLabel>
#include <QMutex>

struct FrameHeader {
    quint16 frameId;          // 帧编号（2字节）
    quint16 fragmentId;       // 当前帧内分片序号（2字节）
    quint32 frameSize;        // 当前帧总字节数（4字节）
};

class UdpReceiver : public QObject
{
    Q_OBJECT

public:
    explicit UdpReceiver(QObject *parent = nullptr);
    ~UdpReceiver();

    bool startListening();
    void stopListening();

    // 获取统计数据
    quint64 getTotalPackets() const { return m_totalPackets; }
    quint64 getTotalFrames() const { return m_totalFrames; }
    double getPacketLossRate() const { return m_packetLossRate; }

signals:
    // 统计信息更新
    // 错误信息
    void errorOccurred(const QString &error);
    void frame_toproess(QByteArray frameData, qint64 udpFirstRecvMs, qint64 udpAssembledMs);

private slots:
    void readPendingDatagrams();
    void updateStatistics();

private:
    QUdpSocket *m_udpSocket;
    quint16 m_port;

    // 帧重组缓冲区
    QMap<quint16, QMap<quint16, QByteArray>> m_frameBuffers; // frameId -> fragmentId -> data
    QMap<quint16, quint32> m_frameSizes; // frameId -> expected total size
    QMap<quint16, qint64> m_frameFirstRecvMs; // frameId -> first UDP fragment receive time (ms)

    // 统计信息
    quint64 m_totalPackets;
    quint64 m_totalFrames;
    quint64 m_lostPackets;
    quint16 m_lastFrameId;
    quint16 m_packetLossRate;
    QTimer *m_statsTimer;
    QLabel *m_displayLabel;
    // 线程安全
    mutable QMutex m_mutex;

    FrameHeader parseHeader(const QByteArray &data);
    void processFrameData(quint16 frameId);
    void cleanupOldFrames();
    QImage frameDecoded(QImage image);

};

#endif // UDPRECEIVER_H
