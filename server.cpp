#include "server.h"
#include <QMessageBox>
#include <QApplication>
#include "message.h"

Server::Server(const QString roomName, QObject *parent): QObject(parent), roomName(roomName)
{
    TCPServer = new QTcpServer(this);
    connect(TCPServer, &QTcpServer::newConnection, this, &newConnection);

    int port = 6000;
    if (TCPServer->listen(QHostAddress::Any, port))
    {
        emit serverStarted(roomName, port);
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
    clientSocket = TCPServer->nextPendingConnection();
    clientSocket->write("Hello. This is room " + roomName.toUtf8() + ".\n");

    connect(clientSocket, QTcpSocket::disconnect, this, &clientDisconnected);
    connect(clientSocket, QTcpSocket::readyRead, this, &readData);

    // Message::display(MessageType::Info, "New Connection", "We've just got a new connection");
}

void Server::readData()
{
    QByteArray data = clientSocket->readAll();
    QString text = QString::fromUtf8(data);


    // Message::display(MessageType::Info, "Message", "Username: " + data);
}




void serverStarted(const QString& roomName, int port)
{

}

// void serverError(const QString& errorMessage);
// void clientConnected(const QString& username);
// void clientDisconnected(const QString& username);
// void dataReceived(const QString& username, const QByteArray& message);















