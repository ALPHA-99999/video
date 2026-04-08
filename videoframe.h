#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include <QByteArray>
#include <QMetaType>

struct VideoFrame
{
    enum PixelFormat {
        PixelFormatInvalid = -1,
        PixelFormatNV12 = 0,
        PixelFormatYUV420P = 1,
        PixelFormatRGB24 = 2
    };

    PixelFormat pixelFormat = PixelFormatInvalid;
    int width = 0;
    int height = 0;
    int linesize[3] = {0, 0, 0};
    QByteArray plane[3];

    qint64 udpFirstRecvMs = 0;
    qint64 udpAssembledMs = 0;
    qint64 decodeStartMs = 0;
    qint64 decodeDoneMs = 0;
    qint64 uiSubmitMs = 0;
};

Q_DECLARE_METATYPE(VideoFrame)

#endif // VIDEOFRAME_H
