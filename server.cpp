#include "server.h"
#include <QMessageBox>
#include <QApplication>
#include "message.h"

server::server(QObject *parent): QObject(parent)
{
    TCPServer = new QTcpServer(this);
    connect(TCPServer, &QTcpServer::newConnection, this, &newConnection);

    int port = 6000;
    if (TCPServer->listen(QHostAddress::Any, port))
        Message::display(MessageType::Info, "Connection", "Listening on port " + QString::number(port));
    else
        Message::display(MessageType::Warning , "Error", "Failed to Listen on port " + QString::number(port));
}

void server::newConnection()
{
    clientSocket = TCPServer->nextPendingConnection();
    connect(clientSocket, QTcpSocket::readyRead, this, &readData);
    Message::display(MessageType::Info, "New Connection", "We've just got a new connection");
}

void server::readData()
{
    QByteArray data = clientSocket->readAll();
    Message::display(MessageType::Info, "Message", "Username: " + data);
}
