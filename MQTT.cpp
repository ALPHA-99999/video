#include "MQTT.h"

MQTT::~MQTT()
{

    if (this->isConnectedToHost()) {
        // 断开所有信号连接，避免析构过程中触发槽函数
        // disconnect(this, nullptr, this, nullptr);
        // qDebug()<<'1';
        // // 断开连接
        disconnectFromHost();

    }
}

void MQTT::onConnected() {

    qDebug() << "connected" ;

     subscribe("CustomControl", 1);


    // // 订阅所有下行消息（服务器 -> 客户端）
     subscribe("GameStatus", 1);

}

void MQTT::onReceived(const QMQTT::Message &message)
{
    QByteArray payload = message.payload();
    std::string data(payload.constData(), payload.size());
    if (message.topic()=="GameStatus")
    {
    if (!GameStatus.ParseFromString(data)) {
        qDebug() << "❌ GameStatus 解析失败";
        return;
    }
       qDebug() <<    data.size();
    }
}

