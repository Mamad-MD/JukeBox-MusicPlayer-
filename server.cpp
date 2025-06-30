#include "server.h"
#include <QMessageBox>
#include <QApplication>
#include "message.h"

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
    instance->deleteLater();
    deletingInProcess = true;
    if (!deletingInProcess)
    {
        qDebug() << "7\n";
        instance = nullptr;
    }
}

void Server::start(const int& port)
{
    if (TCPServer)
    {
        emit serverError("Server already running");
        return;
    }

    qDebug() << "starting\n";
    TCPServer = new QTcpServer(this);
    qDebug() << "new tcp\n";

    if (TCPServer->listen(QHostAddress::Any, port))
    {
        emit serverStarted(port);
        connect(TCPServer, &QTcpServer::newConnection, this, &newConnection);
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
    disconnect(TCPServer, &QTcpServer::newConnection, this, &newConnection);
    for (auto& client : clients)
    {
        if (!client.socket)
            continue;
        qDebug() << "1";
        client.socket->disconnect();
        if (client.socket->state() != QAbstractSocket::UnconnectedState)
                client.socket->disconnectFromHost();
        
        qDebug() << "2";
        client.socket->deleteLater();
        qDebug() << "3";
        client.socket = nullptr;
    }
    clients.clear();
    
    qDebug() << "I'm here";
    
    if (TCPServer)
    {
        qDebug() << "4";
        TCPServer->close();
        qDebug() << "5";
        TCPServer->deleteLater();
        qDebug() << "6";
        TCPServer = nullptr;
    }
    emit serverStopped();
    qDebug() << "8";
    deletingInProcess = false;
}

void Server::newConnection()
{
    QTcpSocket* newClient = TCPServer->nextPendingConnection();
    
    addClient(User(newClient));
    
    newClient->write(roomName.toUtf8());

    // Set up connections for this specific client
    // Note: We need to be careful about how we handle the readyRead signal
    // since multiple clients might send data simultaneously
    connect(newClient, &QTcpSocket::disconnected, this, &clientDisconnected);
    connect(newClient, &QTcpSocket::readyRead, this, &readData);

    // We're gonna emit clientConnected later after we received the username.
}

void Server::readData()
{
    // Here's a crucial concept: we need to figure out which client sent the data
    // The sender() method returns the object that emitted the signal
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());

    if (!senderSocket)
        return;

    QByteArray data = senderSocket->readAll();


    // We'll here expect the client to send a message and say who they are and
    // then we're gonna emit clientConnected signal
    
    // Check if this looks like a username (simple example)
    QString message = QString::fromUtf8(data).trimmed();
    QStringList commandAndArgs = message.split(" ");
    int command = commandAndArgs[0].toInt();
    switch (command)
    {
        case 1: // join request
        {
            QString username = commandAndArgs[1];
            // Now We realize this is the first message this client has sent. So
            // we'll consider it as their username
            for (auto& client : clients)
            {
                if (client.socket == senderSocket)
                {
                    client.username = username;
                    break;
                }
            }
            
            emit clientConnected(message, clients.size());
            
            break;
        }
        case 2: // message
        {
            QString username = commandAndArgs[1];
            bool isFound = false;
            for (auto& client : clients)
            {
                if (client.username == username)
                {
                    isFound = true;
                    break;
                }
            }
            if (isFound)
            {
                // Now We recognize this as a message and not a username
                // Do something with this message
                // QString text = username + ": " + commandAndArgs[2] + "\n";

                // emit dataReceived(username, data);
            }

            break;
        }    
    }
}

void Server::clientDisconnected()
{              
    // Again, we need to identify which client disconnected
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if (!disconnectedSocket)
        return;

    QString clientName = findClientBySocket(disconnectedSocket);

    // Remove this client from our collections
    // removeClientBySocket(disconnectedSocket);
    removeClientByUsername(clientName);
    
    emit clientDisconnection(clientName, clients.size());
}

void Server::broadcastMessage(const QString& message, const QTcpSocket* excludedClientSocket)
{
    for (auto& client : clients)
    {
        if (client.socket != excludedClientSocket && client.socket->state() == QTcpSocket::ConnectedState)
            client.socket->write(message.toUtf8());
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

QString Server::findClientBySocket(const QTcpSocket* socket)
{
    for (auto& client : clients)
        if (client.socket == socket)
            return client.username;
    return "-1";
}

// Getters

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
