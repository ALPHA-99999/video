#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QQueue>
#include <decoder.h>
#include "videoglwidget.h"
#include "udpreceiver.h"
 #include "MQTT.h"
#include "mqttframereceiver.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void onFramePresented(qint64 udpFirstRecvMs,
                          qint64 udpAssembledMs,
                          qint64 decodeStartMs,
                          qint64 decodeDoneMs,
                          qint64 uiSubmitMs,
                          qint64 renderDoneMs);
protected:
   // void mousePressEvent(QMouseEvent *event);

   //  void mouseReleaseEvent(QMouseEvent *event);

   //  void keyPressEvent(QKeyEvent *event);


private:
    QThread *m_UDPThread = nullptr;
    QThread *m_DecoderThread = nullptr;
    //QMQTT::Client *m_mqtt=nullptr;
    UdpReceiver *m_Udpreceiver = nullptr;
    MqttFrameReceiver *m_mqttFrameReceiver = nullptr;
    MyDecoder *m_decoder = nullptr;
    MQTT *m_mqtt = nullptr;
    quint64 m_latencyFrameCount = 0;
    QQueue<qint64> m_recentLatencies;
    qint64 m_recentLatencySumMs = 0;
    QQueue<qint64> m_recentAssembleMs;
    QQueue<qint64> m_recentDecoderQueueMs;
    QQueue<qint64> m_recentDecodeMs;
    QQueue<qint64> m_recentUiQueueMs;
    QQueue<qint64> m_recentUiRenderMs;
    qint64 m_recentAssembleSumMs = 0;
    qint64 m_recentDecoderQueueSumMs = 0;
    qint64 m_recentDecodeSumMs = 0;
    qint64 m_recentUiQueueSumMs = 0;
    qint64 m_recentUiRenderSumMs = 0;
    qint64 m_fpsWindowStartMs = 0;
    quint64 m_fpsWindowFrames = 0;
    double m_outputFps = 0.0;
    Ui::Widget *ui;
    QTimer RemoteControl_Timer;
};

#endif // WIDGET_H
