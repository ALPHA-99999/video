#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QEvent>
#include <QFrame>
#include <QComboBox>
#include <QLabel>
#include <QQueue>
#include <QThread>
#include <QTimer>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QString>
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
    void onMqttLoginClicked();
    void onMqttLogoutClicked();
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttError(QMQTT::ClientError error);
    void onMqttLoginTimeout();
    void onUdpDecodedFrame(const VideoFrame &frame);
    void onMqttDecodedFrame(const VideoFrame &frame);
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
    enum class ToastKind {
        Info,
        Success,
        Error
    };

    enum class VideoSource {
        Udp,
        Mqtt
    };

    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void setMqttOverlayVisible(bool visible);
    void setActiveVideoSource(VideoSource source);
    void presentActiveSourceFrame();
    void applyVideoSourceLayout(VideoSource source);
    void showToast(const QString &message, ToastKind kind = ToastKind::Info);
    void updateMqttUiState();
    QString mqttErrorText(QMQTT::ClientError error) const;

    QThread *m_UDPThread = nullptr;
    QThread *m_udpDecoderThread = nullptr;
    QThread *m_mqttDecoderThread = nullptr;
    //QMQTT::Client *m_mqtt=nullptr;
    UdpReceiver *m_Udpreceiver = nullptr;
    MqttFrameReceiver *m_mqttFrameReceiver = nullptr;
    MyDecoder *m_udpDecoder = nullptr;
    MyDecoder *m_mqttDecoder = nullptr;
    MQTT *m_mqtt = nullptr;
    VideoSource m_activeVideoSource = VideoSource::Udp;
    VideoFrame m_lastUdpFrame;
    VideoFrame m_lastMqttFrame;
    bool m_hasLastUdpFrame = false;
    bool m_hasLastMqttFrame = false;
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
    QWidget *m_mqttOverlay = nullptr;
    QFrame *m_mqttCard = nullptr;
    QLabel *m_mqttTitleLabel = nullptr;
    QLabel *m_mqttHintLabel = nullptr;
    QComboBox *m_clientIdCombo = nullptr;
    QPushButton *m_loginButton = nullptr;
    QPushButton *m_logoutButton = nullptr;
    QLabel *m_mqttStatusLabel = nullptr;
    QWidget *m_toastLayer = nullptr;
    QVBoxLayout *m_toastStack = nullptr;
    QTimer RemoteControl_Timer;
    QTimer m_mqttLoginTimeoutTimer;
    bool m_userRequestedLogout = false;
    bool m_mqttOverlayVisible = false;
};

#endif // WIDGET_H
