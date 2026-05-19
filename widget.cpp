#include "widget.h"

#include "ui_widget.h"
#include "udpreceiver.h"
#include "decoder.h"
#include "mqttframereceiver.h"
#include <QApplication>
#include <QDateTime>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QListView>
#include <QVBoxLayout>
#include <QThread>
#include <QWheelEvent>
#include <algorithm>
#include <cstring>

namespace {

void flipPlaneVertically(QByteArray &plane, int linesize, int rowBytes, int rows)
{
    if (plane.isEmpty() || linesize <= 0 || rowBytes <= 0 || rows <= 1) {
        return;
    }

    QByteArray flipped;
    flipped.resize(rowBytes * rows);
    for (int y = 0; y < rows; ++y) {
        const int srcRow = rows - 1 - y;
        std::memcpy(flipped.data() + y * rowBytes,
                    plane.constData() + srcRow * linesize,
                    rowBytes);
    }
    plane = std::move(flipped);
}

VideoFrame makeVerticallyFlippedFrame(const VideoFrame &frame)
{
    VideoFrame flipped = frame;
    switch (frame.pixelFormat) {
    case VideoFrame::PixelFormatNV12:
        flipPlaneVertically(flipped.plane[0], frame.width, frame.width, frame.height);
        flipPlaneVertically(flipped.plane[1], frame.width, frame.width, frame.height / 2);
        break;
    case VideoFrame::PixelFormatYUV420P:
        flipPlaneVertically(flipped.plane[0], frame.width, frame.width, frame.height);
        flipPlaneVertically(flipped.plane[1], frame.width / 2, frame.width / 2, frame.height / 2);
        flipPlaneVertically(flipped.plane[2], frame.width / 2, frame.width / 2, frame.height / 2);
        break;
    case VideoFrame::PixelFormatRGB24:
        flipPlaneVertically(flipped.plane[0], frame.width * 3, frame.width * 3, frame.height);
        break;
    default:
        break;
    }
    return flipped;
}

} // namespace

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    qApp->installEventFilter(this);

    m_UDPThread = new QThread(this);
    m_udpDecoderThread = new QThread(this);
    m_mqttDecoderThread = new QThread(this);

    m_Udpreceiver = new UdpReceiver();
    m_mqttFrameReceiver = new MqttFrameReceiver();
    m_udpDecoder = new MyDecoder();
    m_mqttDecoder = new MyDecoder();
    m_mqtt = new MQTT(EXAMPLE_HOST, EXAMPLE_PORT, this);

    m_Udpreceiver->moveToThread(m_UDPThread);
    m_mqttFrameReceiver->moveToThread(m_UDPThread);
    m_udpDecoder->moveToThread(m_udpDecoderThread);
    m_mqttDecoder->moveToThread(m_mqttDecoderThread);
    qRegisterMetaType<VideoFrame>("VideoFrame");
    qRegisterMetaType<QByteArray>("QByteArray");
    qRegisterMetaType<qint64>("qint64");

    connect(m_Udpreceiver, &UdpReceiver::frame_toproess, m_udpDecoder, &MyDecoder::Decode);
    connect(m_mqtt, &MQTT::customByteBlockReceived, m_mqttFrameReceiver, &MqttFrameReceiver::ingestPacket);
    connect(m_mqttFrameReceiver, &MqttFrameReceiver::frame_toproess, m_mqttDecoder, &MyDecoder::Decode);
   connect(m_UDPThread, &QThread::started, m_Udpreceiver, &UdpReceiver::startListening);
    connect(m_udpDecoder, &MyDecoder::show__frame, this, &Widget::onUdpDecodedFrame);
    connect(m_mqttDecoder, &MyDecoder::show__frame, this, &Widget::onMqttDecodedFrame);
    connect(ui->videoWidget, &VideoGLWidget::framePresented, this, &Widget::onFramePresented);
    connect(m_mqtt, &QMQTT::Client::connected, this, &Widget::onMqttConnected);
    connect(m_mqtt, &QMQTT::Client::disconnected, this, &Widget::onMqttDisconnected);
    connect(m_mqtt, &QMQTT::Client::error, this, &Widget::onMqttError);
    applyVideoSourceLayout(m_activeVideoSource);

    m_mqttOverlay = new QWidget(this);
    m_mqttOverlay->setObjectName("mqttOverlay");
    m_mqttOverlay->setStyleSheet(
        "#mqttOverlay { background: rgba(0, 0, 0, 150); }"
    );

    auto *overlayLayout = new QVBoxLayout(m_mqttOverlay);
    overlayLayout->setContentsMargins(0, 0, 0, 0);

    m_mqttCard = new QFrame(m_mqttOverlay);
    m_mqttCard->setObjectName("mqttCard");
    m_mqttCard->setFixedWidth(620);
    m_mqttCard->setStyleSheet(
        "#mqttCard { background: #11161f; border: 1px solid #334155; border-radius: 20px; }"
        "#mqttCard QLabel { color: #e2e8f0; }"
        "#mqttCard QComboBox { background: #0b1220; color: #e2e8f0; border: 1px solid #334155; border-radius: 12px; padding: 6px 44px 6px 14px; font-size: 18px; min-height: 0px; }"
        "#mqttCard QComboBox:focus { border: 1px solid #60a5fa; }"
        "#mqttCard QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 40px; border-left: 1px solid #334155; background: #0f172a; border-top-right-radius: 12px; border-bottom-right-radius: 12px; }"
        "#mqttCard QComboBox::down-arrow { width: 16px; height: 16px; image: url(:/ui/down_arrow.svg); }"
        "#mqttCard QComboBox QAbstractItemView { background: #0f172a; color: #e2e8f0; selection-background-color: #2563eb; selection-color: white; border: 1px solid #334155; outline: 0; padding: 4px; }"
        "#mqttCard QComboBox QAbstractItemView::item { min-height: 34px; padding: 4px 12px; }"
        "#mqttCard QPushButton { min-height: 44px; min-width: 120px; border-radius: 12px; font-weight: 700; }"
        "#mqttCard QPushButton#loginButton { background: #2563eb; color: white; border: 1px solid #1d4ed8; }"
        "#mqttCard QPushButton#logoutButton { background: #475569; color: #e2e8f0; border: 1px solid #64748b; }"
        "#mqttCard QLabel#mqttStatusLabel { color: #dbeafe; }"
    );

    auto *cardLayout = new QVBoxLayout(m_mqttCard);
    cardLayout->setContentsMargins(28, 28, 28, 28);
    cardLayout->setSpacing(18);

    m_mqttTitleLabel = new QLabel(QStringLiteral("连接机器人"), m_mqttCard);
    m_mqttTitleLabel->setStyleSheet("font-size: 30px; font-weight: 700; color: #f1f5f9;");
    cardLayout->addWidget(m_mqttTitleLabel);

    m_mqttHintLabel = new QLabel(QStringLiteral("选择编号后点击登录。按 Tab 可弹出或收起这个面板。"), m_mqttCard);
    m_mqttHintLabel->setWordWrap(true);
    m_mqttHintLabel->setStyleSheet("color: #94a3b8; font-size: 16px;");
    cardLayout->addWidget(m_mqttHintLabel);

    auto *clientRow = new QHBoxLayout();
    clientRow->setSpacing(12);
    auto *clientLabel = new QLabel(QStringLiteral("编号"), m_mqttCard);
    clientLabel->setStyleSheet("color: #e2e8f0; font-size: 16px;");
    clientRow->addWidget(clientLabel);

    m_clientIdCombo = new QComboBox(m_mqttCard);
    m_clientIdCombo->setEditable(false);
    m_clientIdCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_clientIdCombo->setMinimumWidth(260);
    m_clientIdCombo->setFixedHeight(42);
    m_clientIdCombo->setMaxVisibleItems(12);
    m_clientIdCombo->setView(new QListView(m_clientIdCombo));
    // 这里按常见分组映射：R1-R6 -> 1-6，B1-B6 -> 101-106。
    m_clientIdCombo->addItem(QStringLiteral("R1"), 1);
    m_clientIdCombo->addItem(QStringLiteral("R2"), 2);
    m_clientIdCombo->addItem(QStringLiteral("R3"), 3);
    m_clientIdCombo->addItem(QStringLiteral("R4"), 4);
    m_clientIdCombo->addItem(QStringLiteral("R5"), 5);
    m_clientIdCombo->addItem(QStringLiteral("R6"), 6);
    m_clientIdCombo->addItem(QStringLiteral("B1"), 101);
    m_clientIdCombo->addItem(QStringLiteral("B2"), 102);
    m_clientIdCombo->addItem(QStringLiteral("B3"), 103);
    m_clientIdCombo->addItem(QStringLiteral("B4"), 104);
    m_clientIdCombo->addItem(QStringLiteral("B5"), 105);
    m_clientIdCombo->addItem(QStringLiteral("B6"), 106);
    m_clientIdCombo->setCurrentIndex(2);
    clientRow->addWidget(m_clientIdCombo, 1);
    cardLayout->addLayout(clientRow);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(12);
    m_loginButton = new QPushButton(QStringLiteral("登录"), m_mqttCard);
    m_loginButton->setObjectName("loginButton");
    m_logoutButton = new QPushButton(QStringLiteral("退出"), m_mqttCard);
    m_logoutButton->setObjectName("logoutButton");
    buttonRow->addWidget(m_loginButton);
    buttonRow->addWidget(m_logoutButton);
    buttonRow->addStretch(1);
    cardLayout->addLayout(buttonRow);

    m_mqttStatusLabel = new QLabel(QStringLiteral("MQTT: disconnected"), m_mqttCard);
    m_mqttStatusLabel->setObjectName("mqttStatusLabel");
    m_mqttStatusLabel->setWordWrap(true);
    m_mqttStatusLabel->setStyleSheet("font-size: 16px;");
    cardLayout->addWidget(m_mqttStatusLabel);

    overlayLayout->addStretch(1);
    auto *centerRow = new QHBoxLayout();
    centerRow->addStretch(1);
    centerRow->addWidget(m_mqttCard);
    centerRow->addStretch(1);
    overlayLayout->addLayout(centerRow);
    overlayLayout->addStretch(1);

    m_toastLayer = new QWidget(this);
    m_toastLayer->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_toastLayer->setObjectName("toastLayer");
    auto *toastLayout = new QVBoxLayout(m_toastLayer);
    toastLayout->setContentsMargins(24, 18, 24, 0);
    toastLayout->setSpacing(10);
    toastLayout->setAlignment(Qt::AlignTop);
    m_toastStack = new QVBoxLayout();
    m_toastStack->setSpacing(10);
    m_toastStack->setAlignment(Qt::AlignTop);
    toastLayout->addLayout(m_toastStack);
    toastLayout->addStretch(1);

    connect(m_loginButton, &QPushButton::clicked, this, &Widget::onMqttLoginClicked);
    connect(m_logoutButton, &QPushButton::clicked, this, &Widget::onMqttLogoutClicked);

    m_mqttLoginTimeoutTimer.setSingleShot(true);
    m_mqttLoginTimeoutTimer.setInterval(5000);
    connect(&m_mqttLoginTimeoutTimer, &QTimer::timeout, this, &Widget::onMqttLoginTimeout);

    connect(&RemoteControl_Timer, &QTimer::timeout, this,[this](){
        QMQTT::Message  Message;
        Message.setQos(1);
        Message.setTopic("CustomControl");
        m_mqtt->CustomControl.set_data("222");
        std::string data;
        m_mqtt->CustomControl.SerializeToString(&data);
        QByteArray payload(data.data(), data.size());
        Message.setPayload(payload);
        if (m_mqtt->connectionState()==QMQTT::ConnectionState::STATE_CONNECTED) m_mqtt->publish(Message);
if (m_mqtt->connectionState()!=QMQTT::ConnectionState::STATE_CONNECTED)  m_mqtt->connectToHost();;
    });
  // RemoteControl_Timer.start(1000);

    setMqttOverlayVisible(true);
    updateMqttUiState();
    m_mqttOverlay->setGeometry(rect());
    m_mqttOverlay->raise();
    m_mqttOverlay->show();
    if (m_toastLayer) {
        m_toastLayer->setGeometry(rect());
        m_toastLayer->raise();
        m_toastLayer->show();
    }

    m_UDPThread->start();
    m_udpDecoderThread->start();
    m_mqttDecoderThread->start();
}

Widget::~Widget()
{
    qApp->removeEventFilter(this);
    if (m_mqtt && m_mqtt->isConnectedToHost()) {
        m_mqtt->disconnectFromHost();
    }
    delete ui;
    m_UDPThread->quit();
    m_udpDecoderThread->quit();
    m_mqttDecoderThread->quit();
    m_UDPThread->wait();
    m_udpDecoderThread->wait();
    m_mqttDecoderThread->wait();

    delete m_Udpreceiver;
    delete m_mqttFrameReceiver;
    delete m_udpDecoder;
    delete m_mqttDecoder;
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            QWidget *focus = QApplication::focusWidget();
            if (!focus || this->isAncestorOf(focus) || focus == this) {
                setMqttOverlayVisible(!m_mqttOverlayVisible);
                return true;
            }
        }
        if (keyEvent->key() == Qt::Key_X) {
            const VideoSource nextSource = (m_activeVideoSource == VideoSource::Udp)
                ? VideoSource::Mqtt
                : VideoSource::Udp;
            setActiveVideoSource(nextSource);
            return true;
        }
    } else if (event->type() == QEvent::Wheel) {
        if (m_activeVideoSource == VideoSource::Mqtt && ui && ui->videoWidget && watched == ui->videoWidget) {
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
            const QPoint angleDelta = wheelEvent->angleDelta();
            if (angleDelta.y() != 0) {
                adjustMqttVideoDisplaySize(angleDelta.y() > 0 ? 1 : -1);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_mqttOverlay) {
        m_mqttOverlay->setGeometry(rect());
    }
    if (m_toastLayer) {
        m_toastLayer->setGeometry(rect());
    }
}

void Widget::setMqttOverlayVisible(bool visible)
{
    m_mqttOverlayVisible = visible;
    if (!m_mqttOverlay) {
        return;
    }
    if (visible) {
        m_mqttOverlay->show();
        m_mqttOverlay->raise();
        if (m_clientIdCombo) {
            m_clientIdCombo->setFocus(Qt::TabFocusReason);
            m_clientIdCombo->showPopup();
        }
        setCursor(Qt::ArrowCursor);
    } else {
        if (m_clientIdCombo) {
            m_clientIdCombo->hidePopup();
        }
        m_mqttOverlay->hide();
        if (ui && ui->videoWidget) {
            ui->videoWidget->setFocus(Qt::OtherFocusReason);
        } else {
            setFocus(Qt::OtherFocusReason);
        }
    }
}

void Widget::setActiveVideoSource(VideoSource source)
{
    if (m_activeVideoSource == source) {
        return;
    }

    m_activeVideoSource = source;
    applyVideoSourceLayout(source);
    if (source == VideoSource::Udp) {
        showToast("视频源: UDP", ToastKind::Info);
    } else {
        showToast("视频源: MQTT", ToastKind::Info);
    }
    presentActiveSourceFrame();
}

void Widget::applyVideoSourceLayout(VideoSource source)
{
    if (!ui || !ui->videoWidget || !ui->rootLayout) {
        return;
    }

    if (source == VideoSource::Mqtt) {
        ui->videoWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->videoWidget->setFixedSize(m_mqttDisplaySize, m_mqttDisplaySize);
        ui->rootLayout->setAlignment(ui->videoWidget, Qt::AlignCenter);
    } else {
        ui->videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->videoWidget->setMinimumSize(QSize(0, 0));
        ui->videoWidget->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
        ui->rootLayout->setAlignment(ui->videoWidget, Qt::Alignment());
    }

    ui->videoWidget->updateGeometry();
    ui->rootLayout->invalidate();
    ui->rootLayout->activate();
}

void Widget::adjustMqttVideoDisplaySize(int deltaSteps)
{
    if (m_activeVideoSource != VideoSource::Mqtt || !ui || !ui->videoWidget) {
        return;
    }

    if (deltaSteps == 0) {
        return;
    }

    const int kMinSize = 200;
    const int kMaxSize = 1000;
    const int kStepSize = 40;

    m_mqttDisplaySize = std::clamp(m_mqttDisplaySize + deltaSteps * kStepSize, kMinSize, kMaxSize);
    ui->videoWidget->setFixedSize(m_mqttDisplaySize, m_mqttDisplaySize);
    ui->videoWidget->updateGeometry();
    if (ui->rootLayout) {
        ui->rootLayout->invalidate();
        ui->rootLayout->activate();
    }
}

void Widget::presentActiveSourceFrame()
{
    if (!ui || !ui->videoWidget) {
        return;
    }

    const bool hasFrame = (m_activeVideoSource == VideoSource::Udp)
        ? m_hasLastUdpFrame
        : m_hasLastMqttFrame;
    if (!hasFrame) {
        return;
    }

    if (m_activeVideoSource == VideoSource::Udp) {
        ui->videoWidget->submitFrame(m_lastUdpFrame);
    } else {
        ui->videoWidget->submitFrame(m_lastMqttFrame);
    }
}

void Widget::onUdpDecodedFrame(const VideoFrame &frame)
{
    m_lastUdpFrame = frame;
    m_hasLastUdpFrame = true;
    if (m_activeVideoSource == VideoSource::Udp && ui && ui->videoWidget) {
        ui->videoWidget->submitFrame(frame);
    }
}

void Widget::onMqttDecodedFrame(const VideoFrame &frame)
{
    m_lastMqttFrame = makeVerticallyFlippedFrame(frame);
    m_hasLastMqttFrame = true;
    if (m_activeVideoSource == VideoSource::Mqtt && ui && ui->videoWidget) {
        ui->videoWidget->submitFrame(m_lastMqttFrame);
    }
}

void Widget::showToast(const QString &message, ToastKind kind)
{
    if (!m_toastLayer || !m_toastStack) {
        return;
    }

    auto *toast = new QFrame(m_toastLayer);
    toast->setObjectName("toastCard");
    QString borderColor;
    QString textColor;
    QString backgroundColor;
    switch (kind) {
    case ToastKind::Success:
        borderColor = "#22c55e";
        textColor = "#dcfce7";
        backgroundColor = "#0f172a";
        break;
    case ToastKind::Error:
        borderColor = "#ef4444";
        textColor = "#fee2e2";
        backgroundColor = "#0f172a";
        break;
    case ToastKind::Info:
    default:
        borderColor = "#60a5fa";
        textColor = "#dbeafe";
        backgroundColor = "#0f172a";
        break;
    }

    toast->setStyleSheet(QString(
        "#toastCard { background: %1; border: 1px solid %2; border-radius: 14px; }"
        "#toastCard QLabel { color: %3; font-size: 16px; }"
    ).arg(backgroundColor, borderColor, textColor));

    auto *toastLayout = new QHBoxLayout(toast);
    toastLayout->setContentsMargins(18, 12, 18, 12);
    auto *label = new QLabel(message, toast);
    label->setWordWrap(true);
    toastLayout->addWidget(label);

    m_toastStack->insertWidget(0, toast);
    toast->show();
    toast->raise();

    QTimer::singleShot(3000, toast, [this, toast]() {
        if (!toast) {
            return;
        }
        if (m_toastStack) {
            m_toastStack->removeWidget(toast);
        }
        toast->deleteLater();
    });
}

void Widget::onMqttLoginClicked()
{
    if (!m_clientIdCombo || m_clientIdCombo->currentIndex() < 0) {
        m_mqttStatusLabel->setText("MQTT: 登录失败，未选择编号");
        showToast("MQTT: 登录失败，未选择编号", ToastKind::Error);
        setMqttOverlayVisible(true);
        return;
    }

    const QString label = m_clientIdCombo->currentText().trimmed();
    const QVariant idData = m_clientIdCombo->currentData();
    if (!idData.isValid()) {
        m_mqttStatusLabel->setText("MQTT: 登录失败，编号映射无效");
        showToast("MQTT: 登录失败，编号映射无效", ToastKind::Error);
        setMqttOverlayVisible(true);
        return;
    }
    const QString clientId = idData.toString();

    m_userRequestedLogout = false;
    if (m_mqtt->connectionState() == QMQTT::ConnectionState::STATE_CONNECTED) {
        m_mqtt->disconnectFromHost();
    }

    m_mqtt->setClientId(clientId);
    m_mqtt->connectToHost();
    const QString status = QString("MQTT: 正在登录，%1(%2)").arg(label, clientId);
    m_mqttStatusLabel->setText(status);
    showToast(status, ToastKind::Info);
    m_mqttLoginTimeoutTimer.start();
    updateMqttUiState();
}

void Widget::onMqttLogoutClicked()
{
    m_userRequestedLogout = true;
    m_mqttLoginTimeoutTimer.stop();
    if (m_mqtt->connectionState() != QMQTT::ConnectionState::STATE_DISCONNECTED) {
        m_mqtt->disconnectFromHost();
        m_mqttStatusLabel->setText("MQTT: disconnecting...");
        showToast("MQTT: 正在退出", ToastKind::Info);
    } else {
        m_mqttStatusLabel->setText("MQTT: 已退出");
        showToast("MQTT: 已退出", ToastKind::Info);
    }
    setMqttOverlayVisible(true);
    updateMqttUiState();
}

void Widget::onMqttConnected()
{
    m_mqttLoginTimeoutTimer.stop();
    m_userRequestedLogout = false;
    const QString label = m_clientIdCombo ? m_clientIdCombo->currentText().trimmed() : QString();
    const QString status = label.isEmpty()
        ? QString("MQTT: 登录成功，client_id=%1").arg(m_mqtt->clientId())
        : QString("MQTT: 登录成功，%1(%2)").arg(label, m_mqtt->clientId());
    m_mqttStatusLabel->setText(status);
    showToast(status, ToastKind::Success);
    setMqttOverlayVisible(false);
    updateMqttUiState();
}

void Widget::onMqttDisconnected()
{
    m_mqttLoginTimeoutTimer.stop();
    if (m_userRequestedLogout) {
        m_mqttStatusLabel->setText("MQTT: 已退出");
        showToast("MQTT: 已退出", ToastKind::Info);
    } else if (m_mqtt->connectionState() == QMQTT::ConnectionState::STATE_DISCONNECTED) {
        m_mqttStatusLabel->setText("MQTT: 登录失败或连接已断开");
        showToast("MQTT: 登录失败或连接已断开", ToastKind::Error);
    } else {
        m_mqttStatusLabel->setText("MQTT: disconnected");
        showToast("MQTT: disconnected", ToastKind::Info);
    }
    m_userRequestedLogout = false;
    setMqttOverlayVisible(true);
    updateMqttUiState();
}

void Widget::onMqttError(QMQTT::ClientError error)
{
    m_mqttLoginTimeoutTimer.stop();
    m_userRequestedLogout = false;
    m_mqttStatusLabel->setText(QString("MQTT: 登录失败 - %1").arg(mqttErrorText(error)));
    showToast(QString("MQTT: 登录失败 - %1").arg(mqttErrorText(error)), ToastKind::Error);
    setMqttOverlayVisible(true);
    updateMqttUiState();
}

void Widget::onMqttLoginTimeout()
{
    if (m_mqtt->connectionState() != QMQTT::ConnectionState::STATE_CONNECTED) {
        m_mqttStatusLabel->setText("MQTT: 登录失败，连接超时");
        showToast("MQTT: 登录失败，连接超时", ToastKind::Error);
        if (m_mqtt->connectionState() != QMQTT::ConnectionState::STATE_DISCONNECTED) {
            m_mqtt->disconnectFromHost();
        }
        setMqttOverlayVisible(true);
        updateMqttUiState();
    }
}

void Widget::updateMqttUiState()
{
    const auto state = m_mqtt->connectionState();
    const bool connected = state == QMQTT::ConnectionState::STATE_CONNECTED;
    const bool connecting = state == QMQTT::ConnectionState::STATE_CONNECTING;

    m_loginButton->setEnabled(!connected && !connecting);
    m_logoutButton->setEnabled(connected || connecting);
    m_clientIdCombo->setEnabled(!connecting);
}

QString Widget::mqttErrorText(QMQTT::ClientError error) const
{
    switch (error) {
    case QMQTT::ClientError::UnknownError:
        return "未知错误";
    case QMQTT::ClientError::SocketConnectionRefusedError:
        return "连接被拒绝";
    case QMQTT::ClientError::SocketRemoteHostClosedError:
        return "远端主机关闭连接";
    case QMQTT::ClientError::SocketHostNotFoundError:
        return "未找到主机";
    case QMQTT::ClientError::SocketAccessError:
        return "网络访问错误";
    case QMQTT::ClientError::SocketTimeoutError:
        return "连接超时";
    case QMQTT::ClientError::SocketNetworkError:
        return "网络错误";
    case QMQTT::ClientError::SocketProxyConnectionRefusedError:
        return "代理连接被拒绝";
    case QMQTT::ClientError::SocketProxyConnectionClosedError:
        return "代理连接关闭";
    case QMQTT::ClientError::SocketProxyConnectionTimeoutError:
        return "代理连接超时";
    case QMQTT::ClientError::SocketProxyNotFoundError:
        return "未找到代理";
    case QMQTT::ClientError::SocketProxyProtocolError:
        return "代理协议错误";
    case QMQTT::ClientError::MqttUnacceptableProtocolVersionError:
        return "协议版本不被接受";
    case QMQTT::ClientError::MqttIdentifierRejectedError:
        return "Client ID 被拒绝";
    case QMQTT::ClientError::MqttServerUnavailableError:
        return "服务器不可用";
    case QMQTT::ClientError::MqttBadUserNameOrPasswordError:
        return "用户名或密码错误";
    case QMQTT::ClientError::MqttNotAuthorizedError:
        return "未授权";
    case QMQTT::ClientError::MqttNoPingResponse:
        return "没有 Ping 响应";
    default:
        return "登录失败";
    }
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
        qDebug().noquote() << QString("[FPS] output=%1").arg(QString::number(m_outputFps, 'f', 2));
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
