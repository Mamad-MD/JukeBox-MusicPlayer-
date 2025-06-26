#include "server.h"
#include <QMessageBox>
#include <QApplication>
#include "message.h"

Server::Server(const QString& roomName, QObject *parent): QObject(parent), roomName(roomName)
{
    TCPServer = new QTcpServer(this);
    connect(TCPServer, &QTcpServer::newConnection, this, &newConnection);

    int port = 6000;
    if (TCPServer->listen(QHostAddress::Any, port))
    {
        emit serverStarted(port);
        // Message::display(MessageType::Info, "Connection", "Listening on port " + QString::number(port));
    }
    else
    {
        emit serverError("Failed to listen on port" + QString::number(port));
        // Message::display(MessageType::Warning , "Error", "Failed to Listen on port " + QString::number(port));
    }
}

void Server::newConnection()
{
    QTcpSocket* newClient = TCPServer->nextPendingConnection();
    addClient(User(newClient));
    
    newClient->write(("Hello. This is room " + roomName +
                      ". You are client #" + QString::number(clients.size()) + ".\n").toUtf8());


    // Set up connections for this specific client
    // Note: We need to be careful about how we handle the readyRead signal
    // since multiple clients might send data simultaneously
    connect(newClient, &QTcpSocket::disconnected, this, &clientDisconnected);
    connect(newClient, &QTcpSocket::readyRead, this, &readData);

    // We're gonna emit clientConnected later after we received the username.
    
    // Message::display(MessageType::Info, "New Connection", "We've just got a new connection");
}

void Server::readData()
{
    // Here's a crucial concept: we need to figure out which client sent the data
    // The sender() method returns the object that emitted the signal
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());

    if (!senderSocket) {
        return; // Safety check - this shouldn't happen
    }

    // Read the data from the specific client who sent it
    QByteArray data = senderSocket->readAll();


    // We'll here expect the client to send a message and say who they are and
    // then we're gonna emit clientConnected signal
    
    // Check if this looks like a username (simple example)
    QString message = QString::fromUtf8(data).trimmed();
    bool isFound = false;
    for (auto& client : clients)
    {
        if (client.username == message)
        {
            isFound = true;
            break;
        }
    }
    if (isFound)
    {
        // Now We recognize this as a message and not a username
        // Do something with this message
        QString text = findClientBySocket(senderSocket) + ": " + message + "\n";

        // ======================
        // We're gonna remove the second line and just handle showing the messages using
        // dataReceived signal that a UI Component gets. Basically I'm not gonna deal with
        // showing the message in my server class.
        emit dataReceived(findClientBySocket(senderSocket), data);
        broadcastMessage(text);
    }
    else
    {
        // Now We realize this is the first message this client has sent. So
        // we'll consider it as their username
        for (auto& client : clients)
        {
            if (client.socket == senderSocket)
            {
                client.username = message;
                break;
            }
        }

        emit clientConnected(message, clients.size());
        

        // ======================
        // We're gonna remove the second line and just handle showing the messages using
        // dataReceived signal that a UI Component catches. Basically I'm not gonna deal with
        // showing the message in my server class.
        emit dataReceived("-1", data);
        QString announcement = message + " has joined the room! \n";
        broadcastMessage(announcement);
    }
    
    // Message::display(MessageType::Info, "Message", "Username: " + data);
}

void Server::clientDisconnected()
{              
    // Again, we need to identify which client disconnected
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if (!disconnectedSocket) {
        return;
    }

    QString clientName = findClientBySocket(disconnectedSocket);

    // Remove this client from our collections
    removeClientBySocket(disconnectedSocket);

    // We're gonna move this to another class that's gonna handle our message displaying.
    // Announce to remaining clients that someone left
    QString announcement = clientName + " has left the room!\n";
    broadcastMessage(announcement);

    // Emit signal for UI updates
    emit clientDisconnection(roomName, clients.size());
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
    clients.removeIf([socket](const User& client) {
        return client.socket == socket;
    });
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

QStringList Server::getClientList() const
{
    QStringList usernames;
    for (auto& client : clients)
        usernames.append(client.username);
        
    return usernames;
}

QString Server::getRoomName() const
{
    return roomName;
}
