#include "videoglwidget.h"
#include <QDateTime>
#include <QMutexLocker>
#include <utility>

VideoGLWidget::VideoGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
}

VideoGLWidget::~VideoGLWidget()
{
    makeCurrent();
    releaseTextures();
    doneCurrent();
}

void VideoGLWidget::submitFrame(const VideoFrame &frame)
{
    VideoFrame local = frame;
    local.uiSubmitMs = QDateTime::currentMSecsSinceEpoch();
    {
        QMutexLocker locker(&m_mutex);
        m_pendingFrame = std::move(local);
        m_hasPendingFrame = true;
    }
    update();
}

void VideoGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glDisable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    static const char *kVertexShader = R"(
        #version 330
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTex;
        out vec2 vTex;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            vTex = aTex;
        }
    )";

    static const char *kFragmentShader = R"(
        #version 330
        in vec2 vTex;
        out vec4 FragColor;
        uniform sampler2D texY;
        uniform sampler2D texUV;
        uniform sampler2D texU;
        uniform sampler2D texV;
        uniform sampler2D texRGB;
        uniform int uFormat;
        vec3 yuvToRgb(float y, float u, float v) {
            float r = y + 1.5748 * (v - 0.5);
            float g = y - 0.1873 * (u - 0.5) - 0.4681 * (v - 0.5);
            float b = y + 1.8556 * (u - 0.5);
            return vec3(r, g, b);
        }
        void main() {
            if (uFormat == 0) {
                float y = texture(texY, vTex).r;
                vec2 uv = texture(texUV, vTex).rg;
                FragColor = vec4(yuvToRgb(y, uv.x, uv.y), 1.0);
            } else if (uFormat == 1) {
                float y = texture(texY, vTex).r;
                float u = texture(texU, vTex).r;
                float v = texture(texV, vTex).r;
                FragColor = vec4(yuvToRgb(y, u, v), 1.0);
            } else {
                FragColor = vec4(texture(texRGB, vTex).rgb, 1.0);
            }
        }
    )";

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, kVertexShader);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, kFragmentShader);
    m_program.link();
}

void VideoGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void VideoGLWidget::paintGL()
{
    {
        QMutexLocker locker(&m_mutex);
        if (m_hasPendingFrame) {
            m_renderFrame = std::move(m_pendingFrame);
            m_hasPendingFrame = false;
        }
    }

    if (m_renderFrame.pixelFormat == VideoFrame::PixelFormatInvalid) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    ensureTexturesForFrame(m_renderFrame);
    uploadFrame(m_renderFrame);

    static const float kVertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 0.0f
    };

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program.bind();
    m_program.enableAttributeArray(0);
    m_program.enableAttributeArray(1);
    m_program.setAttributeArray(0, GL_FLOAT, kVertices, 2, 4 * sizeof(float));
    m_program.setAttributeArray(1, GL_FLOAT, kVertices + 2, 2, 4 * sizeof(float));

    m_program.setUniformValue("texY", 0);
    m_program.setUniformValue("texUV", 1);
    m_program.setUniformValue("texU", 2);
    m_program.setUniformValue("texV", 3);
    m_program.setUniformValue("texRGB", 4);

    int formatUniform = 2;
    if (m_renderFrame.pixelFormat == VideoFrame::PixelFormatNV12) {
        formatUniform = 0;
    } else if (m_renderFrame.pixelFormat == VideoFrame::PixelFormatYUV420P) {
        formatUniform = 1;
    }
    m_program.setUniformValue("uFormat", formatUniform);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program.disableAttributeArray(0);
    m_program.disableAttributeArray(1);
    m_program.release();

    const qint64 renderDoneMs = QDateTime::currentMSecsSinceEpoch();
    emit framePresented(m_renderFrame.udpFirstRecvMs,
                        m_renderFrame.udpAssembledMs,
                        m_renderFrame.decodeStartMs,
                        m_renderFrame.decodeDoneMs,
                        m_renderFrame.uiSubmitMs,
                        renderDoneMs);
}

void VideoGLWidget::ensureTexturesForFrame(const VideoFrame &frame)
{
    if (frame.width == m_texWidth && frame.height == m_texHeight && frame.pixelFormat == m_texFormat) {
        return;
    }

    releaseTextures();
    m_texWidth = frame.width;
    m_texHeight = frame.height;
    m_texFormat = frame.pixelFormat;

    if (frame.pixelFormat == VideoFrame::PixelFormatNV12) {
        glGenTextures(1, &m_texY);
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texWidth, m_texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &m_texUV);
        glBindTexture(GL_TEXTURE_2D, m_texUV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_texWidth / 2, m_texHeight / 2, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (frame.pixelFormat == VideoFrame::PixelFormatYUV420P) {
        glGenTextures(1, &m_texY);
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texWidth, m_texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &m_texU);
        glBindTexture(GL_TEXTURE_2D, m_texU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texWidth / 2, m_texHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &m_texV);
        glBindTexture(GL_TEXTURE_2D, m_texV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texWidth / 2, m_texHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (frame.pixelFormat == VideoFrame::PixelFormatRGB24) {
        glGenTextures(1, &m_texRGB);
        glBindTexture(GL_TEXTURE_2D, m_texRGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

void VideoGLWidget::uploadFrame(const VideoFrame &frame)
{
    if (frame.pixelFormat == VideoFrame::PixelFormatNV12) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width, frame.height, GL_RED, GL_UNSIGNED_BYTE, frame.plane[0].constData());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texUV);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width / 2, frame.height / 2, GL_RG, GL_UNSIGNED_BYTE, frame.plane[1].constData());
    } else if (frame.pixelFormat == VideoFrame::PixelFormatYUV420P) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width, frame.height, GL_RED, GL_UNSIGNED_BYTE, frame.plane[0].constData());

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_texU);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width / 2, frame.height / 2, GL_RED, GL_UNSIGNED_BYTE, frame.plane[1].constData());

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_texV);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width / 2, frame.height / 2, GL_RED, GL_UNSIGNED_BYTE, frame.plane[2].constData());
    } else if (frame.pixelFormat == VideoFrame::PixelFormatRGB24) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_texRGB);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.width, frame.height, GL_RGB, GL_UNSIGNED_BYTE, frame.plane[0].constData());
    }
}

void VideoGLWidget::releaseTextures()
{
    if (m_texY) {
        glDeleteTextures(1, &m_texY);
        m_texY = 0;
    }
    if (m_texUV) {
        glDeleteTextures(1, &m_texUV);
        m_texUV = 0;
    }
    if (m_texU) {
        glDeleteTextures(1, &m_texU);
        m_texU = 0;
    }
    if (m_texV) {
        glDeleteTextures(1, &m_texV);
        m_texV = 0;
    }
    if (m_texRGB) {
        glDeleteTextures(1, &m_texRGB);
        m_texRGB = 0;
    }
}
