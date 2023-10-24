#include "clientconnection.h"

ClientConnection::ClientConnection(QObject* parent)
    : socket(new QTcpSocket), QObject(parent)
{
    connect(socket, &QTcpSocket::readyRead, this, &ClientConnection::ReadFromSocket);
}

void ClientConnection::SetSocketDescriptor(qintptr &descriptor)
{
    socket->setSocketDescriptor(descriptor);
}

void ClientConnection::SetNickname(const QString &nickname)
{
    this->nickname = nickname;
}

void ClientConnection::SetLoggedIn(const bool &logged_in)
{
    this->logged_in = logged_in;
}

void ClientConnection::SendUserlist(QVector<ClientConnection *> &clients)
{
    QJsonObject userlist_message;
    QJsonArray userlist_array;
    userlist_message[QStringLiteral("type")] = QStringLiteral("userlist");
    for (auto& client : clients) {
        if (!client->GetNickname().isEmpty()) {
            userlist_array.push_back(QJsonValue(client->GetNickname()));
        }
    }
    userlist_message[QStringLiteral("userlist")] = userlist_array;
    SendMessage(userlist_message);
}

bool ClientConnection::IsLoggedIn()
{
    return logged_in;
}

QString ClientConnection::GetNickname()
{
    return nickname;
}

void ClientConnection::SendMessage(const QJsonObject &message)
{
    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact);
    socket->write(data);
    socket->write("\n");
}

void ClientConnection::ReadFromSocket()
{
    QByteArray byte_array = socket->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(byte_array);
    emit MessageReceived(this, json_doc.object());
}
