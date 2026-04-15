#include "MQTT.h"
#include <QDateTime>
#include <QStringList>

MQTT::~MQTT()
{

    if (this->isConnectedToHost()) {
        disconnectFromHost();
    }
}

void MQTT::onConnected() {

    qDebug() << "MQTT connected" ;
     subscribe("CustomControl", 1);
     subscribe("CustomByteBlock", 1);

    // // 订阅所有下行消息（服务器 -> 客户端）
     subscribe("GameStatus", 1);


}

void MQTT::onReceived(const QMQTT::Message &message)
{
    QByteArray payload = message.payload();
    std::string data(payload.constData(), payload.size());
    if (message.topic()=="CustomByteBlock")
    {
        CustomByteBlock.ParseFromString(data);
        const std::string &blockData = CustomByteBlock.data();
        QByteArray rawBlock(blockData.data(), static_cast<int>(blockData.size()));
        const int previewLength = qMin(10, rawBlock.size());
        emit customByteBlockReceived(rawBlock,
                                     QDateTime::currentMSecsSinceEpoch());
        return;
    }

}
