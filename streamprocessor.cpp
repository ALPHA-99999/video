#include "streamprocessor.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QThread>
#include <QDebug>

// HEVC起始码
static const char HEVC_START_CODE[] = "\x00\x00\x00\x01";
static const int HEVC_START_CODE_LEN = 4;

StreamProcessor::StreamProcessor(QObject *parent)
    : QObject(parent)
    , m_running(false)
{
}

StreamProcessor::~StreamProcessor()
{
    stopProcessing();
}

void StreamProcessor::addFrame(const QByteArray &frameData, quint16 frameId)
{
    if (!m_running) return;

    QMutexLocker locker(&m_queueMutex);

    FramePacket packet;
    packet.data = frameData;
    packet.frameId = frameId;
    packet.timestamp = QDateTime::currentMSecsSinceEpoch();

    m_frameQueue.enqueue(packet);

    // 限制队列大小，防止内存溢出
    if (m_frameQueue.size() > 100) {
        m_frameQueue.dequeue();
        qWarning() << "帧队列溢出，丢弃最旧帧";
    }

    m_queueCondition.wakeOne();
}

void StreamProcessor::startProcessing()
{
    if (m_running) return;

    m_running = true;
    QThread *thread = QThread::create([this]() { processFrames(); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void StreamProcessor::stopProcessing()
{
    m_running = false;
    m_queueCondition.wakeAll();
}

void StreamProcessor::processFrames()
{
    while (m_running) {
        FramePacket packet;

        {
            QMutexLocker locker(&m_queueMutex);

            if (m_frameQueue.isEmpty()) {
                m_queueCondition.wait(&m_queueMutex, 100);
                continue;
            }

            packet = m_frameQueue.dequeue();
        }

        // 验证HEVC帧
        if (!validateHevcFrame(packet.data)) {
            qWarning() << "无效的HEVC帧，Frame ID:" << packet.frameId;
            continue;
        }

        // 提取NALU单元（可根据需要扩展）
        extractHevcNaluUnits(packet.data);

        // 发射处理完成的帧
        emit frameProcessed(packet.data);

        // 保存到文件（调试用）
        if (packet.frameId % 100 == 0) { // 每100帧保存一次
            QString filename = QString("frame_%1_%2.h265")
                                   .arg(packet.frameId, 5, 10, QChar('0'))
                                   .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
            saveFrameToFile(packet.data, filename);
        }
    }
}

bool StreamProcessor::validateHevcFrame(const QByteArray &frameData)
{
    if (frameData.size() < HEVC_START_CODE_LEN) {
        return false;
    }

    // 检查是否有HEVC起始码
    if (frameData.startsWith(HEVC_START_CODE)) {
        return true;
    }

    // 也可以检查其他HEVC特征
    return true; // 简化的验证，实际应用中需要更严格的验证
}

void StreamProcessor::extractHevcNaluUnits(const QByteArray &frameData)
{
    // 这里可以实现HEVC NALU单元的提取和解析
    // 实际应用中可以添加解码、显示等功能

    int pos = 0;
    while (pos < frameData.size()) {
        // 查找起始码
        int nextStart = frameData.indexOf(HEVC_START_CODE, pos + HEVC_START_CODE_LEN);
        if (nextStart == -1) {
            nextStart = frameData.size();
        }

        // 提取NALU
        QByteArray nalu = frameData.mid(pos, nextStart - pos);

        if (!nalu.isEmpty() && nalu.size() > HEVC_START_CODE_LEN) {
            // 可以在这里处理NALU单元
            // 例如：解码、分析等
        }

        pos = nextStart;
    }
}

void StreamProcessor::saveFrameToFile(const QByteArray &frameData, const QString &filename)
{
    QString filepath = QDir::currentPath() + "/frames/" + filename;

    QDir dir(QDir::currentPath() + "/frames");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(frameData);
        file.close();
        emit frameSaved(filepath);
        qDebug() << "帧已保存到:" << filepath;
    } else {
        emit processingError(QString("无法保存帧到文件: %1").arg(file.errorString()));
    }
}
