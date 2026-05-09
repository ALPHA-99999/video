#ifndef DECODER_H
#define DECODER_H

#include <QObject>
#include <QElapsedTimer>
#include "videoframe.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/hwcontext.h>
#include <libswscale/swscale.h>
}

class MyDecoder : public QObject
{
    Q_OBJECT
public:
    explicit MyDecoder(QObject *parent = nullptr);
    ~MyDecoder();
    void Decode(QByteArray frameData, qint64 udpFirstRecvMs, qint64 udpAssembledMs);

private:
    bool initDecoder();
    bool initHardwareDevice();
    void disableHardwareDecoding();
    bool extractFramePlanes(AVFrame *frame, VideoFrame &outFrame);
    static enum AVPixelFormat getHwFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    bool m_decoderInitialized = false;
    const AVCodec *m_codec = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    AVPacket *m_packet = nullptr;
    AVFrame *m_frame = nullptr;
    enum AVPixelFormat m_hwPixFmt = AV_PIX_FMT_NONE;
    AVHWDeviceType m_hwDeviceType = AV_HWDEVICE_TYPE_NONE;
    AVBufferRef *m_hwDeviceCtx = nullptr;
    AVFrame *m_swFrame = nullptr;
    bool m_hwEnabled = false;
    SwsContext *m_rgbSwsCtx = nullptr;
    int m_rgbSwsWidth = 0;
    int m_rgbSwsHeight = 0;
    AVPixelFormat m_rgbSwsSrcFmt = AV_PIX_FMT_NONE;
    quint64 m_decodedFramesSinceLog = 0;
    quint64 m_iFramesSinceLog = 0;
    quint64 m_pFramesSinceLog = 0;
    QElapsedTimer m_decodeLogTimer;

signals:
    void show__frame(const VideoFrame &frame);
};

#endif // DECODER_H
