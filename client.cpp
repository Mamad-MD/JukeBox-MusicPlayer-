#include "client.h"
#include "message.h"

Client::Client(QObject *parent): QObject(parent), lastReceivedMessage("")
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &connected);
    connect(socket, &QTcpSocket::readyRead, this, &readData);

    int port = 6000;
    socket->connectToHost(QHostAddress::Any, port);
}

void Client::connected()
{
    Message::display(MessageType::Info, "Notice", "Connected successfully!");
}

void Client::sendMessage(const QString& message)
{
    socket->write(message.toUtf8());
}

void Client::readData()
{
    QByteArray data = socket->readAll();

    // Message::display(MessageType::Info, "Message", data);
}
