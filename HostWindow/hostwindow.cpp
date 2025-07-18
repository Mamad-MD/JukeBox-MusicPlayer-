#include "hostwindow.h"
#include "../mainwindow.h"
#include "ui_hostwindow.h"
#include "../MessageDisplayer/message_displayer.h"
#include "../Network/ServerLogic/server.h"
#include "../MusicRoom/musicroom.h"
#include "../Login/authmanager.h"
#include <QTimer>


Server* Server::instance = nullptr; 

HostWindow::HostWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HostWindow), server(nullptr), hasSetupUIConnections(false)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
    ui->LineEdit_RoomName->setReadOnly(true);

    QString username = Authmanager::getLoggedInUsername();
    if (!username.isEmpty())
        ui->LineEdit_RoomName->setText(username + "'s Room");
}

HostWindow::~HostWindow()
{
    if (server)
    {
        server->stop();
        Server::deleteInstance();
    }
    delete ui;
}

void HostWindow::on_PushButton_Back_clicked()
{
      qDebug() << "Back button clicked in JoinWindowH";
    ui->PushButton_Back->setEnabled(false);
          qDebug() << "Deleting Client instance...H";
    Server::deleteInstance();
        qDebug() << "Client instance deleted.H";
    QTimer::singleShot(0, this, [this]() {
              qDebug() << "Showing MainWindow and closing JoinWindowH";
        auto mainWindow = new MainWindow;
        mainWindow->show();
        this->close();
    });
}

void HostWindow::on_PushButton_CreateRoom_clicked()
{
    if (ui->LineEdit_RoomName->text().isEmpty() || ui->LineEdit_Port->text().isEmpty())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Room name or port cannot be empty!");
        return;
    }

    bool isEnteredPortANumber = false;
    int enteredPort = ui->LineEdit_Port->text().toInt(&isEnteredPortANumber);
    if (!isEnteredPortANumber || enteredPort > 20000 || enteredPort < 6000)
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Your entered port is invalid or not a number!");
        return;
    }

    QString roomName = ui->LineEdit_RoomName->text();
    
    server = Server::getInstance(roomName);
    qDebug() << "got the instance";
    connectServerSignalsToUISlots(server);
    server->start(enteredPort);
}

void HostWindow::updateClientsList()
{
    qDebug() << "updateRoomsList\n";
    qDebug() << server;
    qDebug() << QString::number(server->getClientCount());

    ui->TableWidget_Clients->setRowCount(server->getClientCount());
    ui->TableWidget_Clients->setColumnCount(3);
    ui->TableWidget_Clients->setHorizontalHeaderLabels(QStringList() << "Username" << "Join Time" << "Socket");

    for (int i = 0; i < server->getClientCount(); ++i) 
    {
        User& client = server->getClientList()[i];

        ui->TableWidget_Clients->setItem(i, 0, new QTableWidgetItem(client.getUsername()));
        ui->TableWidget_Clients->setItem(i, 1, new QTableWidgetItem(client.getJoinTime().toString()));
        QString socketInfo = QString("%1:%2")
        .arg(client.getSocket()->peerAddress().toString())
        .arg(client.getSocket()->peerPort());
        ui->TableWidget_Clients->setItem(i, 2, new QTableWidgetItem(socketInfo));
    }
}

void HostWindow::on_serverStarted(int port)
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Server started listening on port " + QString::number(port));
}

void HostWindow::on_serverError(const QString& errorMessage)
{
    MessageDisplayer::display(MessageType::Critical, "Error", errorMessage);
    ui->PushButton_GoToMusicRoom->setEnabled(false);
}

void HostWindow::on_clientConnected(const QString& username, int clientsCount)
{
    updateClientsList();
}

void HostWindow::on_clientDisconnection(const QString& username, int clientsCount)
{
    MessageDisplayer::display(MessageType::Info, "Disconnection", username + " disconnected!");
    updateClientsList();
}

void HostWindow::on_dataReceived(const QString& username, const QByteArray& message)
{
    MessageDisplayer::display(MessageType::Info, "Data Received", username);
}

void HostWindow::on_serverStopped()
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Server Stopped!");
    ui->PushButton_GoToMusicRoom->setEnabled(false);
    updateClientsList();
}

void HostWindow::on_serverDeleted()
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Server Object was deleted!");
    ui->PushButton_GoToMusicRoom->setEnabled(false);
    updateClientsList();
}

void HostWindow::on_clientConnectedToMainServer()
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Somebody connected to the main server!");
}

void HostWindow::on_messageReceived(const QString& username, const QString& content)
{

}

void HostWindow::connectServerSignalsToUISlots(const Server* server)
{
    if (hasSetupUIConnections)
        return;
    hasSetupUIConnections = true;
    connect(server, Server::serverStarted, this, &HostWindow::on_serverStarted);
    connect(server, Server::serverError, this, &HostWindow::on_serverError);
    connect(server, Server::clientConnected, this, &HostWindow::on_clientConnected);
    connect(server, Server::clientDisconnection, this, &HostWindow::on_clientDisconnection);
    connect(server, Server::dataReceived, this, &HostWindow::on_dataReceived);
    connect(server, Server::serverStopped, this, &HostWindow::on_serverStopped);
    connect(server, Server::serverDeleted, this, &HostWindow::on_serverDeleted);
    connect(server, Server::clientConnectedToMainServer, this, &HostWindow::on_clientConnectedToMainServer);
    connect(server, Server::messageReceived, this, &HostWindow::on_messageReceived);
    qDebug() << "Connected signals and slots\n";
}


void HostWindow::on_PushButton_GoToMusicRoom_clicked()
{
    if (server)
    {
        Command command(CommandType::GoToMusicRoom, "", "");
        server->broadcastMessage(command);
        
        MusicRoom* musicroom = new MusicRoom(NetworkMode::Server, server, nullptr);
        musicroom->show();
        this->close();
    }
    else
    {
        MusicRoom* musicroom = new MusicRoom(NetworkMode::Offline, nullptr, nullptr);
        musicroom->show();
        this->close();
    }
}


void HostWindow::on_LineEdit_RoomName_editingFinished()
{

}

