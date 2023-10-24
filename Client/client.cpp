#include "client.h"

Client::Client(QObject* qobject) : QObject(qobject), socket(new QTcpSocket(this)) {
    qintptr i=65;
    socket->setSocketDescriptor(i);
    connect(socket, &QTcpSocket::readyRead, this, &Client::ReadFromSocket);
}

Client::~Client()
{
    QDataStream stream(socket);
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("user left");
    message[QStringLiteral("nickname")] = nickname;
    QByteArray byte_array = QJsonDocument(message).toJson();
    socket->write(byte_array);
    socket->flush();
    socket->deleteLater();
}

void Client::ConnectToServer(const QString& address, const quint16& port)
{
    socket->connectToHost(address, port);
    if (socket->state() != QAbstractSocket::ConnectedState) {

        emit ConnectionError();

    } else {
        qDebug() << "Connected!";
    }
}

void Client::SetNickname(const QString& nickname)
{
    this->nickname = nickname;
}

void Client::ReadFromSocket()
{
    QString text = socket->readAll();
    QTextStream stream(&text);
    while (!stream.atEnd()) {
        QByteArray byte_array(stream.readLine().toUtf8());
        QJsonParseError parse_error;
        QJsonDocument json_message = QJsonDocument::fromJson(byte_array, &parse_error);
        if (parse_error.error == QJsonParseError::NoError) {
            ReadJson(json_message.object());
        } else {
            qDebug() << "parse error!";
            qDebug () << parse_error.error;
        }
    }
}

void Client::ReadJson(const QJsonObject &json_object)
{

    QJsonValue type = json_object.value("type");
    qDebug() << type.toString();
    if (type.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) {

        QJsonValue login_result = json_object.value(QLatin1String("result"));
        QJsonValue nickname_value = json_object.value(QLatin1String("nickname"));
        if (login_result.toString().compare(QLatin1String("success"), Qt::CaseInsensitive) == 0) {

            SetNickname(nickname_value.toString());
            emit LoginSuccess();

        } else if (login_result.toString().compare(QLatin1String("incorrect password"), Qt::CaseInsensitive) == 0) {

            emit LoginIncorrectPassword();

        } else if (login_result.toString().compare(QLatin1String("no nickname in database"), Qt::CaseInsensitive) == 0) {

            emit LoginNoNickname();

        } else if (login_result.toString().compare(QLatin1String("user already logged"), Qt::CaseInsensitive) == 0) {

            emit LoginAlreadyLogged(nickname_value.toString());
        }

    } else if (type.toString().compare(QLatin1String("register"), Qt::CaseInsensitive) == 0) {

        QJsonValue register_result = json_object.value(QLatin1String("result"));
        if (register_result.toString().compare(QLatin1String("success"), Qt::CaseInsensitive) == 0 ) {

            emit RegisterSuccess();

        } else if (register_result.toString().compare(QLatin1String("nickname already exists"), Qt::CaseInsensitive) == 0) {

            emit RegisterNicknameExists();

        }

    } else if (type.toString().compare(QLatin1String("user left"), Qt::CaseInsensitive) == 0) {

        QJsonValue user_nickname = json_object.value(QLatin1String("nickname"));
        emit UserLeft(user_nickname.toString());

    } else if (type.toString().compare(QLatin1String("user joined"), Qt::CaseInsensitive) == 0) {

        QJsonValue user_nickname = json_object.value(QLatin1String("nickname"));
        emit UserJoined(user_nickname.toString());

    } else if (type.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) {

        QJsonValue sender_nickname = json_object.value(QLatin1String("nickname"));
        QJsonValue message_text = json_object.value(QLatin1String("text"));
        QJsonValue message_time = json_object.value(QLatin1String("time"));
        emit MessageReceived(sender_nickname.toString(), message_text.toString(), message_time.toString());

    } else if (type.toString().compare(QLatin1String("userlist"), Qt::CaseInsensitive) == 0) {

        QJsonValue userlist = json_object.value(QLatin1String("userlist"));
        QJsonArray userlist_array = userlist.toArray();
        for (const auto& username : userlist_array) {
            emit AddToUserlist(username.toString());
        }

    } else if (type.toString().compare(QLatin1String("chat messages"), Qt::CaseInsensitive) == 0) {

        QJsonValue messages = json_object.value(QLatin1String("chat messages"));
        QJsonArray messages_array = messages.toArray();
        for (int i = 0; i < messages_array.size(); ++i) {

            QJsonObject message = messages_array[i].toObject();
            QJsonValue message_type = message.value(QLatin1String("type"));

            if (message_type.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) {

                emit MessageReceived(message.value(QLatin1String("nickname")).toString(),
                                     message.value(QLatin1String("text")).toString(),
                                     message.value(QLatin1String("time")).toString());

            } else if (message_type.toString().compare(QLatin1String("user joined"), Qt::CaseInsensitive) == 0) {

                emit AddUserJoinedMessageToView(message.value(QLatin1String("nickname")).toString());

            } else if (message_type.toString().compare(QLatin1String("user left"), Qt::CaseInsensitive) == 0) {

                emit AddUserLeftMessageToView(message.value(QLatin1String("nickname")).toString());

            }
        }

    }
}

void Client::Login(const QString &nickname, const QString &password)
{
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("login");
    message[QStringLiteral("nickname")] = nickname;
    message[QStringLiteral("password")] = password;
    QByteArray byte_array = QJsonDocument(message).toJson();
    byte_array.append("\n");
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(byte_array);
    }

}

void Client::Register(const QString &nickname, const QString &password)
{
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("register");
    message[QStringLiteral("nickname")] = nickname;
    message[QStringLiteral("password")] = password;
    QByteArray byte_array = QJsonDocument(message).toJson();
    byte_array.append("\n");
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(byte_array);
    }
}

void Client::SendMessage(const QString &time, const QString &text)
{
    QDataStream stream(socket);
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("message");
    message[QStringLiteral("nickname")] = nickname;
    message[QStringLiteral("text")] = text;
    message[QStringLiteral("time")] = time;
    QByteArray byte_array = QJsonDocument(message).toJson();
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(byte_array);
    }
}
