#ifndef VIDEOGLWIDGET_H
#define VIDEOGLWIDGET_H

#include <QMutex>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include "videoframe.h"

class VideoGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit VideoGLWidget(QWidget *parent = nullptr);
    ~VideoGLWidget() override;

public slots:
    void submitFrame(const VideoFrame &frame);

signals:
    void framePresented(qint64 udpFirstRecvMs,
                        qint64 udpAssembledMs,
                        qint64 decodeStartMs,
                        qint64 decodeDoneMs,
                        qint64 uiSubmitMs,
                        qint64 renderDoneMs);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void ensureTexturesForFrame(const VideoFrame &frame);
    void uploadFrame(const VideoFrame &frame);
    void releaseTextures();

    QMutex m_mutex;
    VideoFrame m_pendingFrame;
    bool m_hasPendingFrame = false;
    VideoFrame m_renderFrame;

    QOpenGLShaderProgram m_program;
    GLuint m_texY = 0;
    GLuint m_texUV = 0;
    GLuint m_texU = 0;
    GLuint m_texV = 0;
    GLuint m_texRGB = 0;
    int m_texWidth = 0;
    int m_texHeight = 0;
    VideoFrame::PixelFormat m_texFormat = VideoFrame::PixelFormatInvalid;
};

#endif // VIDEOGLWIDGET_H
