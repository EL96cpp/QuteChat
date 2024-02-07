#include "server.h"

Server::Server() : sql_service(new SqlService) {

    sql_service->LoadMessages(max_number_of_messages, messages);

    if (this->listen(QHostAddress::Any, 4000)) {

        qDebug() << "Start server!";

    }
}

void Server::MessageFromLoggedIn(ClientConnection *sender, const QJsonObject &json_message) {

    QJsonValue type_value = json_message.value(QLatin1String("type"));

    if (type_value.toString().compare(QLatin1String("user left"), Qt::CaseInsensitive) == 0) {

        for (auto& client : clients) {

            if (QString::compare(client->GetNickname(), sender->GetNickname(), Qt::CaseInsensitive) == 0) {

                clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
                break;

            }
        }

        if (messages.size() >= max_number_of_messages) {

            messages.erase(messages.begin());

        }

        messages.push_back(json_message);
        sql_service->AddMessage(json_message);
        MessageAllClients(json_message);

    }

    if (type_value.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) {

        if (messages.size() >= max_number_of_messages) {

            messages.erase(messages.begin());

        }
        QJsonValue text = json_message.value(QLatin1String("text"));
        QJsonValue nickname = json_message.value(QLatin1String("nickname"));
        QJsonValue time = json_message.value(QLatin1String("time"));
        QJsonObject message;
        message[QStringLiteral("type")] = QStringLiteral("message");
        message[QStringLiteral("text")] = text.toString();
        message[QStringLiteral("time")] = time.toString();
        message[QStringLiteral("nickname")] = nickname.toString();
        messages.push_back(message);
        sql_service->AddMessage(message);
        MessageAllClients(message);

    }
}

void Server::MessageFromLoggedOut(ClientConnection *sender, const QJsonObject &json_message) {

    QJsonValue type_value = json_message.value(QLatin1String("type"));

    if (type_value.toString().compare(QLatin1String("login")) == 0) {

        QString nickname = json_message.value(QLatin1String("nickname")).toString();
        QString password = json_message.value(QLatin1String("password")).toString();
        QJsonObject response;
        response[QStringLiteral("type")] = QStringLiteral("login");

        if (CheckUserLoggedIn(nickname)) {

            response[QStringLiteral("result")] = QStringLiteral("user already logged");
            response[QStringLiteral("nickname")] = nickname;
            sender->SendMessage(response);
            return;

        }

        LoginResult login_result = sql_service->Login(nickname, password);

        switch(login_result) {


            case LoginResult::NO_NICKNAME_IN_DATABASE:
                response[QStringLiteral("result")] = QStringLiteral("no nickname in database");
                sender->SendMessage(response);
                break;

            case LoginResult::INCORRECT_PASSWORD:
                response[QStringLiteral("result")] = QStringLiteral("incorrect password");
                sender->SendMessage(response);
                break;

            case LoginResult::SUCCESS:
                response[QStringLiteral("result")] = QStringLiteral("success");
                response[QStringLiteral("nickname")] = nickname;
                sender->SendMessage(response);
                QJsonObject message;
                message[QStringLiteral("type")] = QStringLiteral("user joined");
                message[QStringLiteral("nickname")] = nickname;
                sql_service->AddMessage(message);
                MessageAllClients(message);

                if (messages.size() >= max_number_of_messages) {

                    messages.erase(messages.begin());

                }
                messages.push_back(message);

                sender->SetLoggedIn(true);
                sender->SetNickname(nickname);
                sender->SendUserlist(clients);
                SendChatMessages(sender);
                break;
        }

    } else if (type_value.toString().compare(QLatin1String("register")) == 0) {

        QString nickname = json_message.value(QStringLiteral("nickname")).toString();
        QString password = json_message.value(QStringLiteral("password")).toString();
        RegisterResult register_result = sql_service->Register(nickname, password);
        QJsonObject response;
        response[QStringLiteral("type")] = QStringLiteral("register");

        switch (register_result) {
            case RegisterResult::NICKNAME_ALREADY_EXISTS:
                response[QStringLiteral("result")] = QStringLiteral("nickname already exists");
                sender->SendMessage(response);
                break;

            case RegisterResult::SUCCESS:
                response[QStringLiteral("result")] = QStringLiteral("success");
                sender->SendMessage(response);
                break;
        }

    } else if (type_value.toString().compare(QLatin1String("user left")) == 0) {

        for (auto& client : clients) {

            if (client == sender) {

                clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());

            }
        }

        if (messages.size() >= max_number_of_messages) {

            messages.erase(messages.begin());

        }

        sql_service->AddMessage(json_message);
        messages.push_back(json_message);
        MessageAllClients(json_message);
    }
}

void Server::MessageAllClients(const QJsonObject &message) {

    for (auto& client : clients) {

        if (client->IsLoggedIn()) {

            client->SendMessage(message);

        }
    }
}

void Server::SendChatMessages(ClientConnection* client) {

    QJsonObject chat_messages;
    QJsonArray messages_array;
    chat_messages[QStringLiteral("type")] = QStringLiteral("chat messages");

    for (auto& message : messages) {

        messages_array.push_back(message);

    }

    chat_messages[QStringLiteral("chat messages")] = messages_array;
    client->SendMessage(chat_messages);

}

bool Server::CheckUserLoggedIn(const QString &nickname) {

    for (auto& client : clients) {

        if (QString::compare(client->GetNickname(), nickname, Qt::CaseInsensitive) == 0) {

            return true;

        }
    }

    return false;
}

void Server::MessageReceived(ClientConnection* client, const QJsonObject& json_message) {

    if (client->IsLoggedIn()) {

        MessageFromLoggedIn(client, json_message);

    } else {

        MessageFromLoggedOut(client, json_message);

    }

}

void Server::incomingConnection(qintptr handle) {

    ClientConnection* client = new ClientConnection(this);
    client->SetSocketDescriptor(handle);
    connect(client, &ClientConnection::MessageReceived, this, &Server::MessageReceived);
    clients.push_back(std::move(client));

}
