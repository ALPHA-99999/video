#ifndef MQTTFRAMERECEIVER_H
#define MQTTFRAMERECEIVER_H

#include <QObject>
#include <QByteArray>
#include <QMap>

struct MqttFrameHeader {
    quint16 frameId = 0;
    quint16 fragmentId = 0;
    quint32 frameSize = 0;
};

class MqttFrameReceiver : public QObject
{
    Q_OBJECT
public:
    explicit MqttFrameReceiver(QObject *parent = nullptr);

public slots:
    void ingestPacket(const QByteArray &packet, qint64 recvMs);

signals:
    void frame_toproess(QByteArray frameData, qint64 firstRecvMs, qint64 assembledMs);

private:
    MqttFrameHeader parseHeader(const QByteArray &data) const;
    void processFrameData(quint16 frameId);
    void cleanupExpiredFrames(qint64 nowMs);

private:
    QMap<quint16, QMap<quint16, QByteArray>> m_frameBuffers;
    QMap<quint16, quint32> m_frameSizes;
    QMap<quint16, qint64> m_frameFirstRecvMs;
    static constexpr qint64 kFrameTimeoutMs = 10000;
    static constexpr quint32 kMaxFrameSize = 1024 * 1024;
};

#endif // MQTTFRAMERECEIVER_H
