#ifndef MQTT_H
#define MQTT_H
#include "qmqtt.h"
#include <QObject>
#include <QTimer>
#include <QApplication>
#include <QByteArray>
 #include <command.pb.h>
const QHostAddress EXAMPLE_HOST("192.168.12.1");
const quint16 EXAMPLE_PORT = 3333;

class MQTT : public QMQTT::Client
{
    Q_OBJECT
public:
    explicit MQTT(const QHostAddress& host = EXAMPLE_HOST,
                        const quint16 port = EXAMPLE_PORT,
                        QObject* parent = nullptr)
        : QMQTT::Client(host, port, parent)

    {
        this->setClientId("3");
        this->setUsername("33");
        this->setCleanSession(true);
        connect(this, &MQTT::connected, this, &MQTT::onConnected);
        connect(this, &MQTT::subscribed, this, &MQTT::onSubscribed);
        connect(this, &MQTT::published, this, &MQTT::onPublished);
        connect(this, &MQTT::received, this, &MQTT::onReceived);
        connect(this, &MQTT::disconnected, this, &MQTT::onDisconnected);
    }
    ~MQTT() override;

public slots:
    void onConnected();

    void onSubscribed(const QString& topic) {qDebug()  << "subscribed " << topic ;}

    void onPublished(const QMQTT::Message& message){qDebug()  << "published " << message.topic() ;}

    void onReceived(const QMQTT::Message& message);

    void onDisconnected() {qDebug() << "Disconnected" ;}
public:
   robomaster::GameStatus GameStatus;
   robomaster::CustomControl  CustomControl;
   robomaster::CustomByteBlock CustomByteBlock;

signals:
   void customByteBlockReceived(const QByteArray &payload, qint64 recvMs);

};



#endif // MQTT_H
