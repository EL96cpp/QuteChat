#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QObject* qobject);
    ~Client();
    void ConnectToServer(const QString& address, const quint16& port);
    void SetNickname(const QString& nickname);

signals:
    void ConnectionError();
    void LoginSuccess();
    void LoginNoNickname();
    void LoginIncorrectPassword();
    void LoginAlreadyLogged(const QString& nickname);
    void RegisterSuccess();
    void RegisterNicknameExists();
    void UserJoined(const QString& nickname);
    void UserLeft(const QString& nickname);
    void MessageReceived(const QString& nickname, const QString& text, const QString& time);
    void AddToUserlist(const QString& nickname);
    void AddUserJoinedMessageToView(const QString& nickname);
    void AddUserLeftMessageToView(const QString& nickname);

public slots:
    void ReadFromSocket();
    void ReadJson(const QJsonObject& json_doc);
    void Login(const QString& nickname, const QString& password);
    void Register(const QString& nickname, const QString& password);
    void SendMessage(const QString& time, const QString& text);

private:
    QTcpSocket* socket;
    QString nickname;

};

#endif // CLIENT_H
