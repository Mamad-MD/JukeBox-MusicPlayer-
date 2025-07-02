#include "joinwindow.h"
#include "../mainwindow.h"
#include "ui_joinwindow.h"
#include "../Network/ClientLogic/client.h"
#include <QTimer>
#include "../MessageDisplayer/message_displayer.h"
#include <QString>
#include "../MusicRoom/musicroom.h"

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


void JoinWindow::on_PushButton_LookForRooms_clicked()
{
    if (ui->LineEdit_Username->text().isEmpty() || ui->LineEdit_Port->text().isEmpty())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Username or port cannot be empty!");
        return;
    }

    bool isEnteredPortANumber = false;
    int enteredPort = ui->LineEdit_Port->text().toInt(&isEnteredPortANumber);
    if (!isEnteredPortANumber || enteredPort > 20000 || enteredPort < 6000)
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Your entered port is invalid or not a number!");
        return;
    }

    ui->LineEdit_Username->setReadOnly(true);
    
    QString username = ui->LineEdit_Username->text();

    client = Client::getInstance(username);
    
    ui->PushButton_LookForRooms->setEnabled(false);
    ui->PushButton_Back->setEnabled(false);
    
    client->connectToHost(enteredPort);
    connectClientSignalsToUISlots(client); 
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
    connect(client, Client::clientObjectDeleted, this, on_clientObjectDeleted);
    connect(client, Client::goToMusicRoom, this, on_goToMusicRoom);
    
    qDebug() << "Connected signals and slots\n";
}

void JoinWindow::updateRoomsList(const QString& roomName)
{
    ui->TableWidget_Rooms->setRowCount(1);
    ui->TableWidget_Rooms->setColumnCount(1);
    ui->TableWidget_Rooms->setHorizontalHeaderLabels(QStringList() << "Room Name");

    ui->TableWidget_Rooms->setItem(0, 0, new QTableWidgetItem(roomName));
}

// Slots:
void JoinWindow::on_connectedToServer()
{
    // MessageDisplayer::display(MessageType::Info, "Notice", "Connected to the Main Server");
    ui->Label_NetworkStatus->setText("Connected");
    ui->Label_NetworkStatus->setStyleSheet("QLabel { color: green; }");
    ui->PushButton_LookForRooms->setEnabled(true);
    ui->PushButton_Back->setEnabled(true);
}

void JoinWindow::on_ReceivedRoomName(const QString& roomName)
{
    ui->PushButton_LookForRooms->setEnabled(true);
    ui->PushButton_Back->setEnabled(true);
    // MessageDisplayer::display(MessageType::Info, "Notice", "RoomName Received: " + roomName);
    updateRoomsList(roomName);
}

void JoinWindow::on_disconnection()
{
    updateRoomsList("");
    MessageDisplayer::display(MessageType::Info, "Critical", "You got disconnected!");
    ui->PushButton_LookForRooms->setEnabled(true);
    ui->PushButton_Back->setEnabled(true);
}

void JoinWindow::on_dataReceived(const QByteArray& data)
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Received this data: " + QString::fromUtf8(data));
}


void JoinWindow::on_clientError(const QString& errorMessage)
{
    ui->Label_NetworkStatus->setText("Disconnected");
    ui->Label_NetworkStatus->setStyleSheet("QLabel { color: red; }");
    ui->PushButton_LookForRooms->setEnabled(true);
    ui->PushButton_Back->setEnabled(true);
    MessageDisplayer::display(MessageType::Critical, "Error", errorMessage);
}

void JoinWindow::on_clientObjectDeleted()
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Client object deleted!");
}

void JoinWindow::on_TableWidget_Rooms_cellClicked(int row, int column)
{
    Command clientCommand(CommandType::Join_Request, client->username, "");
    QByteArray commandInByteArray = commandToByteArray(&clientCommand);
    client->socket->write(commandInByteArray);
}

void JoinWindow::on_goToMusicRoom()
{
    MusicRoom* musicroom = new MusicRoom(NetworkMode::Client, nullptr, client);
    musicroom->show();
    this->close();
}
