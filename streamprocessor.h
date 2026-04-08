#ifndef STREAMPROCESSOR_H
#define STREAMPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

class StreamProcessor : public QObject
{
    Q_OBJECT

public:
    explicit StreamProcessor(QObject *parent = nullptr);
    ~StreamProcessor();

    void addFrame(const QByteArray &frameData, quint16 frameId);
    void startProcessing();
    void stopProcessing();

    // 保存帧到文件（用于调试）
    void saveFrameToFile(const QByteArray &frameData, const QString &filename);

signals:
    void frameProcessed(const QByteArray &frameData);
    void frameSaved(const QString &filename);
    void processingError(const QString &error);

private:
    struct FramePacket {
        QByteArray data;
        quint16 frameId;
        qint64 timestamp;
    };

    QQueue<FramePacket> m_frameQueue;
    QMutex m_queueMutex;
    QWaitCondition m_queueCondition;
    bool m_running;

    void processFrames();

    // HEVC相关处理（可扩展）
    bool validateHevcFrame(const QByteArray &frameData);
    void extractHevcNaluUnits(const QByteArray &frameData);
};

#endif // STREAMPROCESSOR_H
