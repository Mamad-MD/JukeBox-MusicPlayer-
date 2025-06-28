#include "joinwindow.h"
#include "mainwindow.h"
#include "ui_joinwindow.h"
#include "client.h"
#include <QTimer>
#include "message.h"
#include <QString>

Client* Client::instance = nullptr;

JoinWindow::JoinWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JoinWindow), client(nullptr), hasSetupUIConnections(false)
{

    ui->setupUi(this);

    // updateRoomsList();

    // QTimer* timer = new QTimer(this);

    // connect(timer, &QTimer::timeout, this, &updateRoomsList);
    // timer->start(1000);
}

JoinWindow::~JoinWindow()
{
    delete ui;
}

void JoinWindow::on_PushButton_Back_clicked()
{
    Client::deleteInstance();
    client = nullptr;
    
    auto mainWindow = new MainWindow;
    mainWindow->show();
    this->close();
}

void JoinWindow::updateRoomsList()
{
    qDebug() << "hey\n";
}

void JoinWindow::on_PushButton_LookForRooms_clicked()
{
    if (ui->LineEdit_Username->text().isEmpty() || ui->LineEdit_Port->text().isEmpty())
    {
        Message::display(MessageType::Critical, "Error", "Username or port cannot be empty!");
        return;
    }

    bool isEnteredPortANumber = false;
    int enteredPort = ui->LineEdit_Port->text().toInt(&isEnteredPortANumber);
    if (!isEnteredPortANumber || enteredPort > 20000 || enteredPort < 6000)
    {
        Message::display(MessageType::Critical, "Error", "Your entered port is invalid or not a number!");
        return;
    }

    ui->LineEdit_Username->setReadOnly(true);
    
    QString username = ui->LineEdit_Username->text();
    
    client = Client::getInstance(username);
    
    ui->PushButton_LookForRooms->setEnabled(false);
    ui->PushButton_Back->setEnabled(false);

    client->connectToHost(enteredPort);
    connectClientSignalsToUISlots(client);
    // QTimer::singleShot(1000, this, [=]() {
    // if (client->socket->state() != QAbstractSocket::ConnectedState)
    // {
    //     socket->abort(); // cancel the connection attempt
    //     Message::display(MessageType::Critical, "Notice", "Connection timed out");
    // }
    // });

    
}

void JoinWindow::connectClientSignalsToUISlots(const Client* client)
{
    if (hasSetupUIConnections)
        return;
    hasSetupUIConnections = true;
    connect(client, Client::connectedToServer, this, on_connectedToServer);
    connect(client, Client::receivedRoomName, this, on_ReceivedRoomName);
    connect(client, Client::disconnection, this, on_disconnection);
    connect(client, Client::dataReceived, this, on_dataReceived);
    connect(client, Client::clientError, this, on_clientError);
    
    qDebug() << "Connected signals and slots\n";
}

// Slots:
void JoinWindow::on_connectedToServer()
{
    Message::display(MessageType::Info, "Notice", "Connected to the Main Server");
}

void JoinWindow::on_ReceivedRoomName(const QString& roomName)
{
    ui->PushButton_LookForRooms->setEnabled(true);
    ui->PushButton_Back->setEnabled(true);
    Message::display(MessageType::Info, "Notice", "RoomName Received: " + roomName);
}

void JoinWindow::on_disconnection()
{
    Message::display(MessageType::Info, "Critical", "You got disconnected!");
}

void JoinWindow::on_dataReceived(const QByteArray& data)
{
    Message::display(MessageType::Info, "Notice", "Received this data: " + QString::fromUtf8(data));
}


void JoinWindow::on_clientError(const QString& errorMessage)
{
    ui->PushButton_LookForRooms->setEnabled(true);
    ui->PushButton_Back->setEnabled(true);
    Message::display(MessageType::Critical, "Error", errorMessage);
}

