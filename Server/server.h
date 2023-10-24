#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
#include <QVector>

#include "clientconnection.h"
#include "sqlservice.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void MessageFromLoggedIn(ClientConnection* sender, const QJsonObject& json_data);
    void MessageFromLoggedOut(ClientConnection* sender, const QJsonObject& json_data);
    void MessageAllClients(const QJsonObject& message);
    void SendChatMessages(ClientConnection* client);
    bool CheckUserLoggedIn(const QString& nickname);

public slots:
    void MessageReceived(ClientConnection* sender, const QJsonObject& json_message);

private:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

private:
    SqlService* sql_service;
    QVector<ClientConnection*> clients;
    QVector<QJsonObject> messages;
    int max_number_of_messages = 500;
};

#endif // SERVER_H
