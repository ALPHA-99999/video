#include "widget.h"

#include "ui_widget.h"
#include "udpreceiver.h"
#include "decoder.h"
#include "Qthread.h"
#include <QDateTime>
#include <algorithm>
#include <QEvent>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    m_UDPThread = new QThread(this);
    m_DecoderThread = new QThread(this);


    m_Udpreceiver = new UdpReceiver();
    m_decoder = new MyDecoder();
    m_mqtt = new MQTT(EXAMPLE_HOST, EXAMPLE_PORT, this);

    m_Udpreceiver->moveToThread(m_UDPThread);
    m_decoder->moveToThread(m_DecoderThread);
    qRegisterMetaType<VideoFrame>("VideoFrame");

    connect(m_Udpreceiver, &UdpReceiver::frame_toproess, m_decoder, &MyDecoder::Decode);
    connect(m_UDPThread, &QThread::started, m_Udpreceiver, &UdpReceiver::startListening);
    connect(m_decoder, &MyDecoder::show__frame, ui->videoWidget, &VideoGLWidget::submitFrame);
    connect(ui->videoWidget, &VideoGLWidget::framePresented, this, &Widget::onFramePresented);


   m_mqtt->connectToHost();
  //  m_mqtt->disconnectFromHost();
//     connect(&RemoteControl_Timer, &QTimer::timeout, this,[this](){
//         // /::Message  Message;
//         // Message.setQos(1);
//         // Message.setTopic("RemoteControl");
//         // std::string data;
//         // //m_mqtt->RemoteControl.SerializeToString(&data);
//         // QByteArray payload(data.data(), data.size());
//         // Message.setPayload(payload);
//         // if (m_mqtt->connectionState()==QMQTT::ConnectionState::STATE_CONNECTED) m_mqtt->publish(Message);
// if (m_mqtt->connectionState()!=QMQTT::ConnectionState::STATE_CONNECTED)  m_mqtt->connectToHost();;
//     });
   RemoteControl_Timer.start(15);

    m_UDPThread->start();
    m_DecoderThread->start();

     setMouseTracking(true);
setFocusPolicy(Qt::ClickFocus);

grabMouse(); // 捕获鼠标
    // QCursor::setCursor(Qt::BlankCursor);

    // // 2. 启用鼠标跟踪

    // grabMouse(); // 捕获鼠标
    // auto widgetCenter = this->rect().center();
    // QPoint globalCenter = this->mapToGlobal(widgetCenter);

    // // 4. 移动光标到中心
    // QCursor::setPos(globalCenter);

}

Widget::~Widget()
{
    delete ui;
    m_UDPThread->quit();
        m_DecoderThread->quit();
}

void Widget::onFramePresented(qint64 udpFirstRecvMs,
                              qint64 udpAssembledMs,
                              qint64 decodeStartMs,
                              qint64 decodeDoneMs,
                              qint64 uiSubmitMs,
                              qint64 renderDoneMs)
{
    constexpr int kLatencyWindowSize = 120;
    const qint64 assembleMs = std::max<qint64>(0, udpAssembledMs - udpFirstRecvMs);
    const qint64 decoderQueueMs = std::max<qint64>(0, decodeStartMs - udpAssembledMs);
    const qint64 decodeMs = std::max<qint64>(0, decodeDoneMs - decodeStartMs);
    const qint64 uiQueueMs = std::max<qint64>(0, uiSubmitMs - decodeDoneMs);
    const qint64 uiRenderMs = std::max<qint64>(0, renderDoneMs - uiSubmitMs);
    const qint64 endToEndLatencyMs = std::max<qint64>(0, renderDoneMs - udpFirstRecvMs);
    if (m_fpsWindowStartMs == 0) {
        m_fpsWindowStartMs = renderDoneMs;
    }
    m_fpsWindowFrames++;
    const qint64 fpsElapsedMs = renderDoneMs - m_fpsWindowStartMs;
    if (fpsElapsedMs >= 1000) {
        m_outputFps = static_cast<double>(m_fpsWindowFrames) * 1000.0 / static_cast<double>(fpsElapsedMs);
        m_fpsWindowStartMs = renderDoneMs;
        m_fpsWindowFrames = 0;
    }

    auto pushWindow = [kLatencyWindowSize](QQueue<qint64> &queue, qint64 &sum, qint64 value) {
        queue.enqueue(value);
        sum += value;
        if (queue.size() > kLatencyWindowSize) {
            sum -= queue.dequeue();
        }
    };

    m_latencyFrameCount++;
    pushWindow(m_recentLatencies, m_recentLatencySumMs, endToEndLatencyMs);
    pushWindow(m_recentAssembleMs, m_recentAssembleSumMs, assembleMs);
    pushWindow(m_recentDecoderQueueMs, m_recentDecoderQueueSumMs, decoderQueueMs);
    pushWindow(m_recentDecodeMs, m_recentDecodeSumMs, decodeMs);
    pushWindow(m_recentUiQueueMs, m_recentUiQueueSumMs, uiQueueMs);
    pushWindow(m_recentUiRenderMs, m_recentUiRenderSumMs, uiRenderMs);

    if (m_latencyFrameCount % 30 == 0) {
        const int windowSize = m_recentLatencies.size();
        auto avgWindow = [windowSize](qint64 sum) -> double {
            return windowSize > 0 ? static_cast<double>(sum) / static_cast<double>(windowSize) : 0.0;
        };
        const double avgLatencyMs = avgWindow(m_recentLatencySumMs);
        const double avgAssembleMs = avgWindow(m_recentAssembleSumMs);
        const double avgDecoderQueueMs = avgWindow(m_recentDecoderQueueSumMs);
        const double avgDecodeMs = avgWindow(m_recentDecodeSumMs);
        const double avgUiQueueMs = avgWindow(m_recentUiQueueSumMs);
        const double avgUiRenderMs = avgWindow(m_recentUiRenderSumMs);

        QVector<qint64> sortedLatencies;
        sortedLatencies.reserve(windowSize);
        for (qint64 v : m_recentLatencies) {
            sortedLatencies.push_back(v);
        }
        std::sort(sortedLatencies.begin(), sortedLatencies.end());

        qint64 p95LatencyMs = 0;
        if (!sortedLatencies.isEmpty()) {
            const int p95Index = (sortedLatencies.size() - 1) * 95 / 100;
            p95LatencyMs = sortedLatencies[p95Index];
        }

        QString bottleneck = "assemble";
        double bottleneckMs = avgAssembleMs;
        if (avgDecoderQueueMs > bottleneckMs) { bottleneck = "decoder_queue"; bottleneckMs = avgDecoderQueueMs; }
        if (avgDecodeMs > bottleneckMs) { bottleneck = "decode"; bottleneckMs = avgDecodeMs; }
        if (avgUiQueueMs > bottleneckMs) { bottleneck = "ui_queue"; bottleneckMs = avgUiQueueMs; }
        if (avgUiRenderMs > bottleneckMs) { bottleneck = "ui_render"; bottleneckMs = avgUiRenderMs; }

        qDebug() << "[Latency] frames:" << m_latencyFrameCount
                 << "current(ms):" << endToEndLatencyMs
                 << "assemble(avg):" << avgAssembleMs
                 << "decoder_queue(avg):" << avgDecoderQueueMs
                 << "decode(avg):" << avgDecodeMs
                 << "ui_queue(avg):" << avgUiQueueMs
                 << "ui_render(avg):" << avgUiRenderMs
                 << "output_fps:" << m_outputFps
                 << "bottleneck:" << bottleneck << bottleneckMs
                 << "win:" << windowSize;
        // qDebug()<< "decode(avg):" << avgDecodeMs;
    }

}

// void Widget::mousePressEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton)               //鼠标左键
//     {
//       //  m_mqtt->RemoteControl.set_left_button_down(1);                     //点击点在窗口上的相对坐标
//        // qDebug()<<"anxia";
//     }
//     else if  (event->button() == Qt::RightButton)
//     {
//        // m_mqtt->RemoteControl.set_right_button_down(1);

//     }
//     QWidget::mousePressEvent(event);
// }
// void Widget::mouseReleaseEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton)               //鼠标左键
//     {
//       //  m_mqtt->RemoteControl.set_left_button_down(0);                     //点击点在窗口上的相对坐标
//         //qDebug()<<"shifang";
//     }
//     else if  (event->button() == Qt::RightButton)
//     {
//         //m_mqtt->RemoteControl.set_right_button_down(0);

//     }
//     QWidget::mouseReleaseEvent(event);
// }
// void Widget::keyPressEvent(QKeyEvent *event)
// {
//   //  m_mqtt->RemoteControl.set_keyboard_value(event->key());
//     if (event->key()==16777216) exit(0);

//     QWidget::keyPressEvent(event);
// }


