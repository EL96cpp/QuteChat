#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontDatabase>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTime>
#include <QList>

#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void Login(const QString& nickname, const QString& password);
    void Register(const QString& nickname, const QString& password);
    void SendMessage(const QString& time, const QString& text);

public slots:
    void UserJoined(const QString& nickname);
    void UserLeft(const QString& nickname);
    void AddMessageToView(const QString& nickname, const QString& text, const QString& time);
    void ConnectionError();
    void LoginSuccess();
    void LoginIncorrectPassword();
    void LoginNoNickname();
    void LoginAlreadyLogged(const QString& nickname);
    void RegisterSuccess();
    void RegisterNicknameExists();
    void AddToUserlist(const QString& nickname);
    void AddUserJoinedMessageToView(const QString& nickname);
    void AddUserLeftMessageToView(const QString& nickname);

private slots:
    void on_StartLogButton_clicked();

    void on_StartRegButton_clicked();

    void on_RegReturnButton_clicked();

    void on_LogReturnButton_clicked();

    void on_RegRegButton_clicked();

    void on_LogLogButton_clicked();

    void on_ChatSendButton_clicked();

    void on_chat_message_edit_returnPressed();

private:
    void SetFont();

private:
    Ui::MainWindow *ui;
    Client* client;
    QStandardItemModel* userlist_model;
    QStandardItemModel* chat_model;
};
#endif // MAINWINDOW_H
