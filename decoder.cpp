#include "decoder.h"
#include "qdebug.h"
#include <QDateTime>
#include <cstring>

namespace {
bool isHardwarePixelFormatForDevice(AVHWDeviceType deviceType, AVPixelFormat pixelFormat)
{
    switch (deviceType) {
    case AV_HWDEVICE_TYPE_D3D11VA:
        return pixelFormat == AV_PIX_FMT_D3D11 || pixelFormat == AV_PIX_FMT_D3D11VA_VLD;
    case AV_HWDEVICE_TYPE_DXVA2:
        return pixelFormat == AV_PIX_FMT_DXVA2_VLD;
    default:
        return false;
    }
}

const char *hwDeviceTypeName(AVHWDeviceType type)
{
    switch (type) {
    case AV_HWDEVICE_TYPE_D3D11VA:
        return "D3D11VA";
    case AV_HWDEVICE_TYPE_DXVA2:
        return "DXVA2";
    default:
        return "Software";
    }
}

inline void copyPlaneTight(QByteArray &dst, const uint8_t *src, int srcLinesize, int rowBytes, int rows)
{
    if (!src || rowBytes <= 0 || rows <= 0) {
        dst.clear();
        return;
    }
    dst.resize(rowBytes * rows);
    uint8_t *out = reinterpret_cast<uint8_t *>(dst.data());
    for (int y = 0; y < rows; ++y) {
        std::memcpy(out + y * rowBytes, src + y * srcLinesize, rowBytes);
    }
}
}

MyDecoder::MyDecoder(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<VideoFrame>("VideoFrame");
    if (!initDecoder()) {
        qWarning() << "HEVC解码器初始化失败";
    }
}

MyDecoder::~MyDecoder()
{
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_swFrame) {
        av_frame_free(&m_swFrame);
    }
    if (m_hwDeviceCtx) {
        av_buffer_unref(&m_hwDeviceCtx);
    }
    if (m_rgbSwsCtx) {
        sws_freeContext(m_rgbSwsCtx);
    }
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
    }
}

enum AVPixelFormat MyDecoder::getHwFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
{
    auto *decoder = static_cast<MyDecoder *>(ctx->opaque);
    if (!decoder) {
        return pix_fmts[0];
    }

    for (const enum AVPixelFormat *fmt = pix_fmts; *fmt != AV_PIX_FMT_NONE; ++fmt) {
        if (decoder->m_hwEnabled &&
            isHardwarePixelFormatForDevice(decoder->m_hwDeviceType, *fmt)) {
            decoder->m_hwPixFmt = *fmt;
            return *fmt;
        }
    }

    while (*pix_fmts != AV_PIX_FMT_NONE) {
        if (*pix_fmts != AV_PIX_FMT_D3D11 &&
            *pix_fmts != AV_PIX_FMT_D3D11VA_VLD &&
            *pix_fmts != AV_PIX_FMT_DXVA2_VLD) {
            return *pix_fmts;
        }
        ++pix_fmts;
    }

    decoder->disableHardwareDecoding();
    qWarning() << "当前硬解像素格式不可用，自动切换软件像素格式";
    return AV_PIX_FMT_NONE;
}

void MyDecoder::disableHardwareDecoding()
{
    m_hwEnabled = false;
    m_hwPixFmt = AV_PIX_FMT_NONE;
    m_hwDeviceType = AV_HWDEVICE_TYPE_NONE;
    if (m_codecCtx) {
        m_codecCtx->opaque = nullptr;
        m_codecCtx->get_format = nullptr;
        if (m_codecCtx->hw_device_ctx) {
            av_buffer_unref(&m_codecCtx->hw_device_ctx);
        }
    }
    if (m_hwDeviceCtx) {
        av_buffer_unref(&m_hwDeviceCtx);
    }
}

bool MyDecoder::initHardwareDevice()
{
    struct HardwareCandidate {
        AVHWDeviceType deviceType;
        AVPixelFormat pixelFormat;
    };

    static const HardwareCandidate kCandidates[] = {
        {AV_HWDEVICE_TYPE_D3D11VA, AV_PIX_FMT_D3D11},
        {AV_HWDEVICE_TYPE_DXVA2, AV_PIX_FMT_DXVA2_VLD},
    };

    for (const HardwareCandidate &candidate : kCandidates) {
        AVBufferRef *deviceCtx = nullptr;
        const int ret = av_hwdevice_ctx_create(&deviceCtx, candidate.deviceType, nullptr, nullptr, 0);
        if (ret < 0) {
            qWarning() << "创建" << hwDeviceTypeName(candidate.deviceType) << "硬件设备失败:" << ret;
            continue;
        }

        AVBufferRef *codecHwCtx = av_buffer_ref(deviceCtx);
        if (!codecHwCtx) {
            qWarning() << "绑定" << hwDeviceTypeName(candidate.deviceType) << "设备到解码器失败";
            av_buffer_unref(&deviceCtx);
            continue;
        }

        m_hwDeviceCtx = deviceCtx;
        m_codecCtx->hw_device_ctx = codecHwCtx;
        m_hwDeviceType = candidate.deviceType;
        m_hwPixFmt = candidate.pixelFormat;
        m_hwEnabled = true;
        m_codecCtx->opaque = this;
        m_codecCtx->get_format = getHwFormat;
        qDebug() << "启用" << hwDeviceTypeName(m_hwDeviceType) << "硬件解码";
        return true;
    }

    disableHardwareDecoding();
    qWarning() << "Windows硬件解码初始化失败，回退软件HEVC解码";
    return false;
}

bool MyDecoder::initDecoder()
{
    m_codec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
    if (!m_codec) {
        qWarning() << "找不到HEVC解码器";
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(m_codec);
    if (!m_codecCtx) {
        qWarning() << "无法分配解码器上下文";
        return false;
    }

    initHardwareDevice();

    m_codecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
    m_codecCtx->flags2 |= AV_CODEC_FLAG2_FAST;

    int openRet = avcodec_open2(m_codecCtx, m_codec, nullptr);
    if (openRet < 0 && m_hwEnabled) {
        qWarning() << "无法打开" << hwDeviceTypeName(m_hwDeviceType) << "硬件解码器，回退软件HEVC:" << openRet;
        disableHardwareDecoding();
        openRet = avcodec_open2(m_codecCtx, m_codec, nullptr);
    }
    if (openRet < 0) {
        qWarning() << "无法打开HEVC解码器";
        avcodec_free_context(&m_codecCtx);
        return false;
    }

    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_swFrame = av_frame_alloc();
    if (!m_packet || !m_frame || !m_swFrame) {
        qWarning() << "解码缓存分配失败";
        return false;
    }

    m_decoderInitialized = true;
    return true;
}

void MyDecoder::Decode(QByteArray frameData, qint64 udpFirstRecvMs, qint64 udpAssembledMs)
{
    if (!m_decoderInitialized || !m_packet || !m_frame) {
        qWarning() << "解码器未初始化";
        return;
    }

    av_packet_unref(m_packet);
    if (av_new_packet(m_packet, frameData.size()) < 0) {
        qWarning() << "AVPacket内存分配失败";
        return;
    }
    std::memcpy(m_packet->data, frameData.constData(), frameData.size());

    int ret = avcodec_send_packet(m_codecCtx, m_packet);
    if (ret == AVERROR(EAGAIN)) {
        qWarning() << "缓冲区已满，需要读取掉一些解码后的音视频帧";
    } else if (ret < 0) {
        qWarning() << "发送packet到解码器失败";
        return;
    }

    while (true) {
        av_frame_unref(m_frame);
        ret = avcodec_receive_frame(m_codecCtx, m_frame);
        if (ret == 0) {
            VideoFrame outFrame;
            outFrame.udpFirstRecvMs = udpFirstRecvMs;
            outFrame.udpAssembledMs = udpAssembledMs;
            outFrame.decodeStartMs = QDateTime::currentMSecsSinceEpoch();
            if (extractFramePlanes(m_frame, outFrame)) {
                outFrame.decodeDoneMs = QDateTime::currentMSecsSinceEpoch();
                emit show__frame(outFrame);
            }
            continue;
        }
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }
        qWarning() << "解码错误:" << ret;
        break;
    }
}

bool MyDecoder::extractFramePlanes(AVFrame *frame, VideoFrame &outFrame)
{
    AVFrame *srcFrame = frame;
    if (m_hwEnabled && static_cast<AVPixelFormat>(frame->format) == m_hwPixFmt) {
        if (!m_swFrame) {
            qWarning() << "无法分配软件帧用于硬件数据拷贝";
            return false;
        }
        av_frame_unref(m_swFrame);
        int ret = av_hwframe_transfer_data(m_swFrame, frame, 0);
        if (ret < 0) {
            qWarning() << hwDeviceTypeName(m_hwDeviceType) << "硬件帧转软件帧失败:" << ret;
            return false;
        }
        m_swFrame->width = frame->width;
        m_swFrame->height = frame->height;
        m_swFrame->format = AV_PIX_FMT_NV12;
        srcFrame = m_swFrame;
    }

    const int width = srcFrame->width;
    const int height = srcFrame->height;
    if (width <= 0 || height <= 0) {
        return false;
    }

    outFrame.width = width;
    outFrame.height = height;

    const AVPixelFormat fmt = static_cast<AVPixelFormat>(srcFrame->format);
    if (fmt == AV_PIX_FMT_NV12) {
        outFrame.pixelFormat = VideoFrame::PixelFormatNV12;
        outFrame.linesize[0] = width;
        outFrame.linesize[1] = width;
        copyPlaneTight(outFrame.plane[0], srcFrame->data[0], srcFrame->linesize[0], width, height);
        copyPlaneTight(outFrame.plane[1], srcFrame->data[1], srcFrame->linesize[1], width, height / 2);
        return true;
    }

    if (fmt == AV_PIX_FMT_YUV420P || fmt == AV_PIX_FMT_YUVJ420P) {
        outFrame.pixelFormat = VideoFrame::PixelFormatYUV420P;
        outFrame.linesize[0] = width;
        outFrame.linesize[1] = width / 2;
        outFrame.linesize[2] = width / 2;
        copyPlaneTight(outFrame.plane[0], srcFrame->data[0], srcFrame->linesize[0], width, height);
        copyPlaneTight(outFrame.plane[1], srcFrame->data[1], srcFrame->linesize[1], width / 2, height / 2);
        copyPlaneTight(outFrame.plane[2], srcFrame->data[2], srcFrame->linesize[2], width / 2, height / 2);
        return true;
    }

    if (fmt != m_rgbSwsSrcFmt || width != m_rgbSwsWidth || height != m_rgbSwsHeight || !m_rgbSwsCtx) {
        if (m_rgbSwsCtx) {
            sws_freeContext(m_rgbSwsCtx);
            m_rgbSwsCtx = nullptr;
        }
        m_rgbSwsCtx = sws_getContext(
            width, height, fmt,
            width, height, AV_PIX_FMT_RGB24,
            SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
        m_rgbSwsSrcFmt = fmt;
        m_rgbSwsWidth = width;
        m_rgbSwsHeight = height;
    }
    if (!m_rgbSwsCtx) {
        return false;
    }

    outFrame.pixelFormat = VideoFrame::PixelFormatRGB24;
    outFrame.linesize[0] = width * 3;
    outFrame.plane[0].resize(width * height * 3);
    uint8_t *dstData[1] = {reinterpret_cast<uint8_t *>(outFrame.plane[0].data())};
    int dstLinesize[1] = {outFrame.linesize[0]};
    sws_scale(m_rgbSwsCtx, srcFrame->data, srcFrame->linesize, 0, height, dstData, dstLinesize);
    return true;
}
