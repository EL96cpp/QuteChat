#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QDataStream>

class ClientConnection : public QObject
{
    Q_OBJECT
public:
    ClientConnection(QObject* parent);
    void SetSocketDescriptor(qintptr& descriptor);
    void SetNickname(const QString& nickname);
    void SetLoggedIn(const bool& logged_in);
    void SendUserlist(QVector<ClientConnection*>& clients);
    bool IsLoggedIn();
    QString GetNickname();

signals:
    void MessageReceived(ClientConnection* client, const QJsonObject& json_data);

public slots:
    void SendMessage(const QJsonObject& message);
    void ReadFromSocket();

private:
    QTcpSocket* socket;
    bool logged_in = false;
    QString nickname;
};

#endif // CLIENTCONNECTION_H
