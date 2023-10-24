#ifndef SQLSERVICE_H
#define SQLSERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <iostream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

enum class LoginResult {
    SUCCESS,
    NO_NICKNAME_IN_DATABASE,
    INCORRECT_PASSWORD
};

enum class RegisterResult {
    SUCCESS,
    NICKNAME_ALREADY_EXISTS
};

class SqlService : public QObject
{
    Q_OBJECT
public:
    SqlService();
    bool CheckIfNicknameExists(const QString& nickname);
    LoginResult Login(const QString& nickname, const QString& password);
    RegisterResult Register(const QString& nickname, const QString& password);
    void AddMessage(const QJsonObject& message);
    void LoadMessages(int& max_number_of_messages, QVector<QJsonObject>& messages);

private:
    QSqlDatabase data_base;

};

#endif // SQLSERVICE_H
