#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QElapsedTimer>
#include <QTimer>

struct StreamPacketHeader {
    quint32 magic = 0;
    quint8 version = 0;
    quint8 flags = 0;
    quint16 headerSize = 0;
    quint32 packetSeq = 0;
    quint16 payloadLen = 0;
    quint16 syncOffset = 0xFFFF;
};

class UdpReceiver : public QObject
{
    Q_OBJECT

public:
    explicit UdpReceiver(QObject *parent = nullptr);
    ~UdpReceiver();

    bool startListening();
    void stopListening();

    quint64 getTotalPackets() const { return m_totalPackets; }
    quint64 getTotalFrames() const { return m_totalFrames; }
    double getPacketLossRate() const { return m_packetLossRate; }

signals:
    void errorOccurred(const QString &error);
    void frame_toproess(QByteArray frameData, qint64 udpFirstRecvMs, qint64 udpAssembledMs);

private slots:
    void readPendingDatagrams();
    void updateStatistics();

private:
    StreamPacketHeader parseHeader(const QByteArray &data) const;
    void processPacket(const StreamPacketHeader &header, const QByteArray &payload, qint64 recvMs);
    void emitStreamChunk(const QByteArray &payload, qint64 firstRecvMs, qint64 assembledMs);
    void resetStreamState();

private:
    QUdpSocket *m_udpSocket = nullptr;
    quint16 m_port = 3334;

    quint64 m_totalPackets = 0;
    quint64 m_totalFrames = 0;
    quint64 m_lostPackets = 0;
    double m_packetLossRate = 0.0;
    QTimer *m_statsTimer = nullptr;
    quint64 m_packetsSinceLog = 0;
    quint64 m_framesSinceLog = 0;
    QElapsedTimer m_statsWindowTimer;

    bool m_hasExpectedPacketSeq = false;
    quint32 m_expectedPacketSeq = 0;
    bool m_waitingForKeyframe = true;
};

#endif // UDPRECEIVER_H
