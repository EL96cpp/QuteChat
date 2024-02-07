#include "sqlservice.h"

SqlService::SqlService() : data_base(QSqlDatabase::addDatabase("QPSQL")) {
    data_base.setHostName("127.0.0.1");
    data_base.setPort(5432);
    data_base.setDatabaseName("qute_chat");
    data_base.setUserName("postgres");
    data_base.setPassword("postgres");
    bool started = data_base.open();
    qDebug() << "Postgres database started:" << started;
}

bool SqlService::CheckIfNicknameExists(const QString &nickname) {

    QSqlQuery check_nickname_query;
    check_nickname_query.prepare("SELECT EXISTS(SELECT 1 FROM chat_users WHERE nickname = (?))");
    check_nickname_query.addBindValue(nickname);
    check_nickname_query.exec();

    while (check_nickname_query.next()) {

        return check_nickname_query.value(0).toBool();

    }

}

LoginResult SqlService::Login(const QString &nickname, const QString &password) {

    if (CheckIfNicknameExists(nickname)) {

        QSqlQuery check_password_query;
        check_password_query.prepare("SELECT password FROM chat_users WHERE nickname = (?)");
        check_password_query.addBindValue(nickname);
        check_password_query.exec();
        QString correct_password;

        while (check_password_query.next()) {

            correct_password = check_password_query.value(0).toString();

        }

        if (password != correct_password) {

            return LoginResult::INCORRECT_PASSWORD;

        } else {

            return LoginResult::SUCCESS;

        }

    } else {

        return LoginResult::NO_NICKNAME_IN_DATABASE;

    }

}

RegisterResult SqlService::Register(const QString &nickname, const QString &password) {

    if (CheckIfNicknameExists(nickname)) {

        return RegisterResult::NICKNAME_ALREADY_EXISTS;

    } else {

        QSqlQuery register_query;
        register_query.prepare("INSERT INTO chat_users VALUES(?, ?)");
        register_query.addBindValue(nickname);
        register_query.addBindValue(password);

        if (!register_query.exec()) {

            qDebug() << register_query.lastError().text();

        }

        return RegisterResult::SUCCESS;

    }
}

void SqlService::AddMessage(const QJsonObject &message) {

    QSqlQuery add_message_query;
    QByteArray array = QJsonDocument(message).toJson();
    QString string_message(array);
    add_message_query.prepare("INSERT INTO chat_messages VALUES(?, now()::timestamp)");
    add_message_query.addBindValue(string_message);

    if (!add_message_query.exec()) {

        qDebug() << add_message_query.lastError().text();

    }

}

void SqlService::LoadMessages(int &max_number_of_messages, QVector<QJsonObject> &messages) {

    QSqlQuery load_messages_query;
    QVariant max(max_number_of_messages);
    load_messages_query.prepare("WITH x AS (SELECT * FROM chat_messages ORDER BY date DESC LIMIT ?) SELECT * FROM x ORDER BY date ASC");
    load_messages_query.addBindValue(max);
    load_messages_query.exec();

    while (load_messages_query.next()) {

        QByteArray byte_array = load_messages_query.value(0).toByteArray();
        QJsonParseError parse_error;
        QJsonDocument json_message = QJsonDocument::fromJson(byte_array, &parse_error);

        if (parse_error.error == QJsonParseError::NoError) {

            messages.push_back(json_message.object());

        } else {

            qDebug() << "parse error!";
            qDebug () << parse_error.error;

        }

    }

}
