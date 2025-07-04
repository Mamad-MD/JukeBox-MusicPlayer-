#include "server.h"
#include <QMessageBox>
#include <QApplication>
#include "../../MessageDisplayer/message_displayer.h"
#include "../Command/command.h"

bool Server::deletingInProcess = false;

Server::Server(const QString& roomName, QObject *parent): QObject(parent), roomName(roomName), TCPServer(nullptr){}

Server::~Server()
{
    stop();
    emit serverDeleted();
}

Server* Server::getInstance(const QString& roomName)
{
    if (!instance)
        instance = new Server(roomName);
    return instance;
}

void Server::deleteInstance()
{
      qDebug() << "Back button clicked in JoinWindow3ERVER";
    // // instance->deleteLater();
    // // deletingInProcess = true;
    // // if (!deletingInProcess)
    // //     instance = nullptr;
    //   deletingInProcess = true;
    //   instance->deleteLater();
    //   instance = nullptr;
      if (instance)
      {
          instance->stop();  // مطمئن شو سرور متوقف شده
          instance->deleteLater();
          instance = nullptr;
          deletingInProcess = false;
      }
}

void Server::start(const int& port)
{
    if (TCPServer)
    {
        emit serverError("Server already running");
        return;
    }

    TCPServer = new QTcpServer(this);

    if (TCPServer->listen(QHostAddress::Any, port))
    {
        emit serverStarted(port);
        connect(TCPServer, &QTcpServer::newConnection, this, &Server::newConnection);
    }
    else
    {
        delete TCPServer;
        TCPServer = nullptr;
        emit serverError("Failed to listen on port " + QString::number(port));
    }
}

void Server::stop()
{
    //disconnect(TCPServer, &QTcpServer::newConnection, this, &newConnection);
    disconnect(TCPServer, &QTcpServer::newConnection, this, &Server::newConnection);

    for (auto& client : clients)
    {
        if (!client.socket)
            continue;
        client.socket->disconnect();
        if (client.socket->state() != QAbstractSocket::UnconnectedState)
            client.socket->disconnectFromHost();

        client.socket->deleteLater();
        client.socket = nullptr;
    }
    clients.clear();

    if (TCPServer)
    {
        TCPServer->close();
        TCPServer->deleteLater();
        TCPServer = nullptr;
        disconnect(TCPServer, &QTcpServer::newConnection, this, &Server::newConnection);

    }
    emit serverStopped();
    deletingInProcess = false;
}

void Server::newConnection()
{
    QTcpSocket* newClient = TCPServer->nextPendingConnection();
    
    addClient(User(newClient));
    
    Command command(CommandType::RoomName_Sending, "", roomName);
    QByteArray commandInBytes = commandToByteArray(&command);
    newClient->write(commandInBytes);

    connect(newClient, &QTcpSocket::disconnected, this, &Server::clientDisconnected);
    connect(newClient, &QTcpSocket::readyRead, this, &Server::readData);

}

void Server::readData()
{
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());

    if (!senderSocket)
        return;

    QByteArray data = senderSocket->readAll();


    Command command = byteArrayToCommand(&data);
    switch (command.commandType)
    {
        case CommandType::Join_Request:
        {
            QString username = command.username;
            for (auto& client : clients)
            {
                if (client.socket == senderSocket)
                {
                    client.username = username;
                    break;
                }
            }
            
            emit clientConnected(username, clients.size());
            
            break;
        }
        case CommandType::Message:
        {
            emit messageReceived(command.username, command.content);
            broadcastMessage(command);

            break;
        }
        
        case CommandType::JoinedMusicRoom:
            clientsInMusicRoom.append(findClientBySocket(senderSocket));
            if (clientsInMusicRoom.size() == clients.size())
            {
                QString names = getUsernamesAsQString();
                names.append("Server");

                Command command(CommandType::ClientNames_Sending, "", names);
                emit clientsAllJoined();
                broadcastMessage(command);
            }
            break;
            
        case CommandType::Request_For_ClientNames:
        {
            QString names = getUsernamesAsQString();
            names.append("Server");

            Command command(CommandType::Response_For_ClientNames, "", names);
            senderSocket->write(commandToByteArray(&command));
        }
        case CommandType::SetMyFolder:
        {
            auto user = findClientBySocket(senderSocket);
            user->folderPath = command.content;
            
            clientsWhoSetTheirFolder.append(user);
            if (clientsWhoSetTheirFolder.size() == clients.size())
                emit allSetTheirFolders();
            break;
        }

        case CommandType::ThisTrackStatusOnMyPC:
        {
            int hasIt = command.content.toInt();
            if (hasIt)
            {
                clientsWhoHaveTheTrack.append(findClientBySocket(senderSocket));
                if (clientsWhoHaveTheTrack.size() == clients.size())
                    emit allHaveTheTrack();
            }
            else
                emit notHaveTheTrack(senderSocket);

            break;
        }
    }
}

void Server::clientDisconnected()
{              
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if (!disconnectedSocket)
        return;

    QString clientName = findClientBySocket(disconnectedSocket)->username;

    removeClientByUsername(clientName);
    
    emit clientDisconnection(clientName, clients.size());
}

void Server::broadcastMessage(Command& command, const QTcpSocket* excludedClientSocket)
{
    if (clients.size() == 0)
        return;
    QByteArray toBeSent = commandToByteArray(&command);
    for (auto& client : clients)
    {
        if (client.socket != excludedClientSocket && client.socket->state() == QTcpSocket::ConnectedState)
            client.socket->write(toBeSent);
    }
}

void Server::sendMessageToClient(QTcpSocket* client, const QString message)
{
    if (client && client->state() == QTcpSocket::ConnectedState) {
        client->write(message.toUtf8());
    }
}

void Server::removeClientBySocket(QTcpSocket* socket)
{
    for (auto& client : clients)
    {
        if (client.socket == socket)
        {
            client.socket->deleteLater();
            client.socket = nullptr;
            clients.removeOne(client);
            break;
        }
    }
}

void Server::removeClientByUsername(const QString& username)
{
    for (auto& client : clients)
    {
        if (client.username == username)
        {
            client.socket->deleteLater();
            client.socket = nullptr;
            clients.removeOne(client);
            break;
        }
    }
}

void Server::addClient(User client)
{
    client.joinTime = QDateTime::currentDateTime();
    clients.append(client);
}

User* Server::findClientBySocket(const QTcpSocket* socket)
{
    for (auto& client : clients)
        if (client.socket == socket)
            return &client;
    return nullptr;
}


int Server::getClientCount() const
{
    return clients.size();
}

QList<User>& Server::getClientList()
{
    return clients;
}

QString Server::getRoomName() const
{
    return roomName;
}

QString Server::getUsernamesAsQString()
{
    QString names;
    for (auto client : clients)
        names.append(client.getUsername() + " ");
    return names;
}

void Server::sendTrackToClient(QTcpSocket* socket, const QString& filePath)
{
    QFile *file = new QFile(filePath);
    if (file->open(QIODevice::ReadOnly)) 
    {
        QByteArray buffer;
        while (!file->atEnd()) 
        {
            buffer = file->read(4096);
            socket->write(buffer);
            socket->waitForBytesWritten();
        }
        file->close();
    }

    Command cmd(CommandType::SendingDone, "", "");
    socket->write(commandToByteArray(&cmd));
    // MessageDisplayer::display(MessageType::Info, "Notice", "A Client received the track");
}
