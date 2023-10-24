#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
    , client(new Client(this)), userlist_model(new QStandardItemModel(0, 1, this))
    , chat_model(new QStandardItemModel(0, 3, this))
{
    ui->setupUi(this);
    setWindowTitle("Qute Chat");

    client->ConnectToServer("127.0.0.1", 4000);
    SetFont();
    connect(client, &Client::ConnectionError, this, &MainWindow::ConnectionError);
    connect(client, &Client::LoginSuccess, this, &MainWindow::LoginSuccess);
    connect(client, &Client::LoginNoNickname, this, &MainWindow::LoginNoNickname);
    connect(client, &Client::LoginIncorrectPassword, this, &MainWindow::LoginIncorrectPassword);
    connect(client, &Client::LoginAlreadyLogged, this, &MainWindow::LoginAlreadyLogged);
    connect(client, &Client::RegisterSuccess, this, &MainWindow::RegisterSuccess);
    connect(client, &Client::RegisterNicknameExists, this, &MainWindow::RegisterNicknameExists);
    connect(client, &Client::MessageReceived, this, &MainWindow::AddMessageToView);
    connect(client, &Client::AddUserJoinedMessageToView, this, &MainWindow::AddUserJoinedMessageToView);
    connect(client, &Client::AddUserLeftMessageToView, this, &MainWindow::AddUserLeftMessageToView);
    connect(client, &Client::AddToUserlist, this, &MainWindow::AddToUserlist);
    connect(client, &Client::UserJoined, this, &MainWindow::UserJoined);
    connect(client, &Client::UserLeft, this, &MainWindow::UserLeft);
    connect(this, &MainWindow::SendMessage, client, &Client::SendMessage);
    connect(this, &MainWindow::Login, client, &Client::Login);
    connect(this, &MainWindow::Register, client, &Client::Register);

    ui->chat_tableview->horizontalHeader()->setVisible(false);
    ui->chat_tableview->verticalHeader()->setVisible(false);
    ui->chat_tableview->setShowGrid(false);

    ui->chat_userlist_view->setModel(userlist_model);
    ui->chat_tableview->setModel(chat_model);
    ui->log_password_edit->setEchoMode(QLineEdit::Password);
    ui->reg_password_edit->setEchoMode(QLineEdit::Password);
    ui->reg_confirm_password_edit->setEchoMode(QLineEdit::Password);
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}

MainWindow::~MainWindow()
{
    delete client;
    delete chat_model;
    delete userlist_model;
    delete ui;
}

void MainWindow::UserJoined(const QString &nickname)
{
    QStandardItem* nickname_item = new QStandardItem(nickname);
    userlist_model->appendRow(nickname_item);
    AddUserJoinedMessageToView(nickname);
}

void MainWindow::UserLeft(const QString &nickname)
{

    AddUserLeftMessageToView(nickname);

    for (int i = 0; i < userlist_model->rowCount(); ++i) {

        if (QString::compare(userlist_model->item(i)->text(), nickname, Qt::CaseSensitive) == 0) {

            userlist_model->removeRow(i);
            break;

        }
    }
}

void MainWindow::AddMessageToView(const QString &nickname, const QString &text, const QString &time)
{

    QString modified_text = text;
    int new_row_step = 50;
    for (int i = new_row_step; i < text.length(); i += new_row_step + 1) {
        modified_text.insert(i, '\n');
    }

    QList<QStandardItem*> items;
    items.append(new QStandardItem(nickname + ":"));
    items.append(new QStandardItem(modified_text));
    items.append(new QStandardItem(time));

    QFont font;
    font.setBold(true);

    items[0]->setTextAlignment(Qt::AlignTop);
    items[0]->setFont(font);
    items[2]->setTextAlignment(Qt::AlignTop);
    items[2]->setFont(font);
    chat_model->appendRow(items);
    ui->chat_tableview->resizeRowsToContents();

    ui->chat_tableview->setColumnWidth(0, 100);
    ui->chat_tableview->setColumnWidth(1, 550);
    ui->chat_tableview->setColumnWidth(2, 150);
    ui->chat_tableview->resizeRowsToContents();
    ui->chat_tableview->scrollToBottom();
}

void MainWindow::ConnectionError()
{
    int reply = QMessageBox::question(this, "Connection error", "Can't connect to server, do you whant to reconnect?",
                                      QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes) {
        client->ConnectToServer("127.0.0.1", 60000);
    } else {
        qApp->quit();
    }
}

void MainWindow::LoginSuccess()
{
    ui->stackedWidget->setCurrentWidget(ui->ChatPage);
}

void MainWindow::LoginIncorrectPassword()
{
    QMessageBox::warning(this, "Login error!", "Incorrect password!");
}

void MainWindow::LoginNoNickname()
{
    QMessageBox::warning(this, "Login error!", "No such nickname in database!");
}

void MainWindow::LoginAlreadyLogged(const QString &nickname)
{
    QMessageBox::warning(this, "Login error!", "User with nickname " + nickname + " already logged");
}

void MainWindow::RegisterSuccess()
{
    QMessageBox::information(this, "Register success", "You successfully registered in QuteChat!");
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}

void MainWindow::RegisterNicknameExists()
{
    QMessageBox::critical(this, "Register error!", "This nickname is already in use, please, enter another nickname!");
}

void MainWindow::AddToUserlist(const QString &nickname)
{

    QStandardItem* nickname_item = new QStandardItem(nickname);
    userlist_model->appendRow(nickname_item);

}

void MainWindow::AddUserJoinedMessageToView(const QString &nickname)
{

    QList<QStandardItem*> items;
    items.append(new QStandardItem(""));
    items.append(new QStandardItem(nickname + " joined chat"));
    items.append(new QStandardItem(""));
    items[1]->setTextAlignment(Qt::AlignHCenter);
    chat_model->appendRow(items);
    ui->chat_tableview->scrollToBottom();

}

void MainWindow::AddUserLeftMessageToView(const QString &nickname)
{

    QList<QStandardItem*> items;
    items.append(new QStandardItem(""));
    items.append(new QStandardItem(nickname + " left chat"));
    items.append(new QStandardItem(""));
    items[1]->setTextAlignment(Qt::AlignHCenter);
    chat_model->appendRow(items);
    ui->chat_tableview->scrollToBottom();
}

void MainWindow::SetFont()
{
    qint32 font_id = QFontDatabase::addApplicationFont(":/Font.ttf");
    QStringList font_list = QFontDatabase::applicationFontFamilies(font_id);
    QString font_family = font_list.first();
    ui->main_label->setFont(QFont(font_family));
    ui->StartLogButton->setFont(QFont(font_family));
    ui->StartRegButton->setFont(QFont(font_family));

    ui->LogLogButton->setFont(QFont(font_family));
    ui->LogReturnButton->setFont(QFont(font_family));
    ui->log_nickname_label->setFont(QFont(font_family));
    ui->log_password_label->setFont(QFont(font_family));

    ui->RegRegButton->setFont(QFont(font_family));
    ui->RegReturnButton->setFont(QFont(font_family));
    ui->reg_nickname_label->setFont(QFont(font_family));
    ui->reg_password_label->setFont(QFont(font_family));
    ui->reg_confirm_password_label->setFont(QFont(font_family));

    ui->ChatSendButton->setFont(QFont(font_family));
    ui->chat_users_label->setFont(QFont(font_family));
}


void MainWindow::on_StartLogButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->LogPage);
}


void MainWindow::on_StartRegButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->RegPage);
}


void MainWindow::on_RegReturnButton_clicked()
{
    ui->reg_nickname_edit->clear();
    ui->reg_password_edit->clear();
    ui->reg_confirm_password_edit->clear();
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}


void MainWindow::on_LogReturnButton_clicked()
{
    ui->log_nickname_edit->clear();
    ui->log_password_edit->clear();
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}


void MainWindow::on_RegRegButton_clicked()
{
    if (ui->reg_nickname_edit->text().isEmpty() || ui->reg_password_edit->text().isEmpty() ||
        ui->reg_confirm_password_edit->text().isEmpty()) {

        QMessageBox::warning(this, "Registraiton error", "Please, fill out all required fields!");

    } else {

        if (ui->reg_password_edit->text() != ui->reg_confirm_password_edit->text()) {

            QMessageBox::warning(this, "Registration error", "Password and password confirmation are not equal!");

        } else {

            emit Register(ui->reg_nickname_edit->text(), ui->reg_password_edit->text());

        }

    }
}


void MainWindow::on_LogLogButton_clicked()
{
    if (ui->log_nickname_edit->text().isEmpty() || ui->log_password_edit->text().isEmpty()) {
        QMessageBox::warning(this, "Login error", "Please, fill out all required fields!");
    } else {
        emit Login(ui->log_nickname_edit->text(), ui->log_password_edit->text());
    }
}


void MainWindow::on_ChatSendButton_clicked()
{
    if (!ui->chat_message_edit->text().isEmpty()) {
        QString text = ui->chat_message_edit->text();
        emit SendMessage(QTime::currentTime().toString(), text);
        ui->chat_message_edit->clear();
    }
}

