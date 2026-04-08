#ifndef DECODER_H
#define DECODER_H

#ifndef USE_HEVC_QSV_DECODER
#define USE_HEVC_QSV_DECODER 1
#endif

#include <QObject>
#include "videoframe.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#if USE_HEVC_QSV_DECODER
#include <libavutil/hwcontext.h>
#endif
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
    bool extractFramePlanes(AVFrame *frame, VideoFrame &outFrame);
#if USE_HEVC_QSV_DECODER
    static enum AVPixelFormat getHwFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);
    bool initQsvDevice();
#endif

    bool m_decoderInitialized = false;
    const AVCodec *m_codec = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    AVPacket *m_packet = nullptr;
    AVFrame *m_frame = nullptr;
#if USE_HEVC_QSV_DECODER
    enum AVPixelFormat m_hwPixFmt = AV_PIX_FMT_NONE;
    AVBufferRef *m_hwDeviceCtx = nullptr;
    AVFrame *m_swFrame = nullptr;
    bool m_hwEnabled = false;
#endif
    SwsContext *m_rgbSwsCtx = nullptr;
    int m_rgbSwsWidth = 0;
    int m_rgbSwsHeight = 0;
    AVPixelFormat m_rgbSwsSrcFmt = AV_PIX_FMT_NONE;

signals:
    void show__frame(const VideoFrame &frame);
};

#endif // DECODER_H
