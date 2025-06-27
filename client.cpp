#include "client.h"
#include "message.h"

Client::Client(QObject *parent): QObject(parent), lastReceivedMessage(""), hasReceivedRoomName(false)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &connected);
    connect(socket, &QTcpSocket::readyRead, this, &readData);
    connect(socket, &QTcpSocket::disconnected, this, &disconnected);

    int port = 6000;
    socket->connectToHost(QHostAddress::Any, port);
}

// Slots:

void Client::connected()
{
    emit connectedToServer();
    // Message::display(MessageType::Info, "Notice", "Connected to the main server!");
}

void Client::readData()
{
    QByteArray data = socket->readAll();
    
    if (!hasReceivedRoomName)
    {
        hasReceivedRoomName = true;
        emit receivedRoomName(QString::fromUtf8(data));
        return;
    }

    // =====================
    // We'll deal with how to interpret messages later we did this on the server side.
    
    
    // Message::display(MessageType::Info, "Message", data);
}

void Client::disconnected()
{
    emit disconnected();
}

void Client::sendMessage(const QString& message)
{
    socket->write(message.toUtf8());
}

