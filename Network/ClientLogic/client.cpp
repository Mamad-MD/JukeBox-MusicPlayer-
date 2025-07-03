#include "client.h"
#include "../../MessageDisplayer/message_displayer.h"

Client* Client::getInstance(const QString& username)
{
    if (!instance)
        instance = new Client(username);
    return instance;
}

void Client::deleteInstance()
{
    //   qDebug() << "Back button clicked in JoinWindow2";
    // instance->deleteLater();
    // instance = nullptr;
    if (instance)
    {
        instance->disconnect();
        instance->deleteLater();
        instance = nullptr;
    }
}

Client::Client(const QString& username, QObject *parent): QObject(parent), lastReceivedMessage(""), hasReceivedRoomName(false), username(username), socket(nullptr), file(nullptr){}

Client::~Client()
{
    disconnect();
    emit clientObjectDeleted();
}

void Client::connectToHost(const int& port)
{
    qDebug() << "before checking socket";
    if (socket)
    {
        qDebug() << "socket is not null";
        
        if (socket->state() == QAbstractSocket::ConnectedState)
            emit clientError("Already connected!");
        else
            emit clientError("Already trying to connect!");
        return;
    }

    qDebug() << "before making socket";
    socket = new QTcpSocket(this);
    qDebug() << "after making socket";
    
    // connect(socket, &QTcpSocket::connected, this, &connected);
    // connect(socket, &QTcpSocket::readyRead, this, &readData);
    // connect(socket, &QTcpSocket::disconnected, this, &disconnected);
    // connect(socket, &QTcpSocket::errorOccurred, this, &on_errorOccurred);
    qDebug() << connect(socket, &QTcpSocket::connected, this, &Client::connected);
    qDebug() << connect(socket, &QTcpSocket::readyRead, this, &Client::readData);
    qDebug() << connect(socket, &QTcpSocket::disconnected, this, &Client::disconnected);
    qDebug() << connect(socket, &QTcpSocket::errorOccurred, this, &Client::on_errorOccurred);


    socket->connectToHost(QHostAddress::LocalHost, port);
}

void Client::disconnect()
{
    if (socket)
    {
        // socket->abort();
        socket->deleteLater();
        socket = nullptr;
        hasReceivedRoomName = false;
    }
}

// Slots:

void Client::connected()
{
    emit connectedToServer();
}

void Client::readData()
{   
    QByteArray data = socket->readAll();
    Command command = byteArrayToCommand(&data);

    if (folderPath != "" && currentTrackName != "" && !file)
    {
        file = new QFile(QDir(folderPath).filePath(currentTrackName));
        file->open(QIODevice::WriteOnly);
    }

    bool hasSendingDoneEverCalled = false;
    
    switch (command.commandType)
    {
        case CommandType::RoomName_Sending:
        {
            if (hasReceivedRoomName)
                break;
            
            hasReceivedRoomName = true;
            emit receivedRoomName(command.content);
             
            break;
        }
        case CommandType::Message:
        {
            emit newMessageReceived(command.username, command.content);
            break;
        }
        case CommandType::GoToMusicRoom:
        {
            emit goToMusicRoom();
            break;
        }
        case CommandType::Response_For_ClientNames:
        {
            emit clientNamesReceived(command.content);
            break;
        }
        case CommandType::ClientNames_Sending:
        {
            emit clientNamesReceived(command.content);
            break;
        }
        case CommandType::WannaPlayThisTrack:
        {
            if (file)
            {
                delete file;
                file = nullptr;
            }
            currentTrackName = command.content;
            emit signal_sayWhetherIHaveThisTrack(currentTrackName);
            break;
        }
        case CommandType::SendingDone:
        {
            file->close();
            hasSendingDoneEverCalled = true;
            break;
        }
        case CommandType::Play_Request:
            emit playTheTrack();
            break;
        case CommandType::Pause_Request:
            emit pauseTheTrack();
            break;
        default:
        {
            // it means it's music sending.
            file->write(data);
        }
    }

    if (hasSendingDoneEverCalled)
        if (file)
        {
            file->close();
            delete file;
            file = nullptr;
        }
    // =====================
    // We'll deal with how to interpret messages later we did this on the server side.
}

void Client::disconnected()
{
    cleanupSocket();
    emit disconnection();
}

void Client::on_errorOccurred(QAbstractSocket::SocketError socketError)
{
    if (!socket)
        return;
    emit clientError(socket->errorString());
    cleanupSocket();
}

void Client::sendCommand(Command& command)
{
    socket->write(commandToByteArray(&command));
}

void Client::cleanupSocket()
{
    static bool cleanupInProgress = false;
    if (socket && !cleanupInProgress)
    {
        cleanupInProgress = true;
        socket->abort();
        socket->deleteLater();
        socket = nullptr;
        cleanupInProgress = false;
    }
}
