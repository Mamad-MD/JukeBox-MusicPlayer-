#include "hostwindow.h"
#include "mainwindow.h"
#include "ui_hostwindow.h"
#include "message.h"
#include "server.h"

HostWindow::HostWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HostWindow), server(nullptr)
{
    ui->setupUi(this);
}

HostWindow::~HostWindow()
{
    if (server)
        server->stop(); // The pointer will get deleted since it's a smart one.
    delete ui;
}

void HostWindow::on_PushButton_Back_clicked()
{
    auto mainWindow = new MainWindow;
    mainWindow->show();
    this->close();
}

void HostWindow::on_PushButton_CreateRoom_clicked()
{
    if (ui->LineEdit_RoomName->text().isEmpty())
    {
        Message::display(MessageType::Critical, "Error", "Room name cannot be empty!");
        return;
    }

    QString roomName = ui->LineEdit_RoomName->text();

    // if (server)
    //     server->stop();
    
    server = Server::getInstance(roomName);
    connectServerSignalsToUISlots(server);
    server->start();
}

void HostWindow::on_serverStarted(int port)
{
    Message::display(MessageType::Info, "Notice", "Server started listening on port " + QString::number(port));
}

void HostWindow::on_serverError(const QString& errorMessage)
{
    Message::display(MessageType::Critical, "Error", errorMessage);
}

void HostWindow::on_clientConnected(const QString& username, int clientsCount)
{
    Message::display(MessageType::Info, "Connection", username);
}

void HostWindow::on_clientDisconnection(const QString& username, int clientsCount)
{
    Message::display(MessageType::Info, "Disconnection", username);
}

void HostWindow::on_dataReceived(const QString& username, const QByteArray& message)
{
    Message::display(MessageType::Info, "Data Received", username);
}

void HostWindow::on_serverStopped()
{
    Message::display(MessageType::Info, "Notice", "Server Stopped!");
}

void HostWindow::on_serverDeleted()
{
    Message::display(MessageType::Info, "Notice", "Server Object was deleted!");
}

void HostWindow::connectServerSignalsToUISlots(const std::shared_ptr<Server>& server)
{
    Server* normalPointer = server.get();
    connect(normalPointer, Server::serverStarted, this, &on_serverStarted);
    connect(normalPointer, Server::serverError, this, &on_serverError);
    connect(normalPointer, Server::clientConnected, this, &on_clientConnected);
    connect(normalPointer, Server::clientDisconnection, this, &on_clientDisconnection);
    connect(normalPointer, Server::dataReceived, this, &on_dataReceived);
    connect(normalPointer, Server::serverStopped, this, &on_serverStopped);
    connect(normalPointer, Server::serverDeleted, this, &on_serverDeleted);
    qDebug() << "Connected signals and slots\n";
}

