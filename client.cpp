#include "client.h"
#include "message.h"


Client* Client::getInstance(const QString& username)
{
    if (!instance)
        instance = new Client(username);
    return instance;
}

void Client::deleteInstance()
{
    delete instance;
    instance = nullptr;
}

Client::Client(const QString& username, QObject *parent): QObject(parent), lastReceivedMessage(""), hasReceivedRoomName(false), username(username){}

Client::~Client()
{
    disconnect();
    emit clientObjectDeleted();
}

void Client::connectToHost(const int& port)
{
    if (socket)
    {
        emit clientError("Already connected!");
        return;
    }
    
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &connected);
    connect(socket, &QTcpSocket::readyRead, this, &readData);
    connect(socket, &QTcpSocket::disconnected, this, &disconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &on_errorOccurred);

    socket->connectToHost(QHostAddress::LocalHost, port);
}

void Client::disconnect()
{
    if (socket)
    {
        socket->abort();
        delete socket;
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
    
    if (!hasReceivedRoomName)
    {
        hasReceivedRoomName = true;
        emit receivedRoomName(QString::fromUtf8(data));
        socket->write(username.toUtf8());
        return;
    }

    // =====================
    // We'll deal with how to interpret messages later we did this on the server side.
    
    
    // Message::display(MessageType::Info, "Message", data);
}

void Client::disconnected()
{
    emit disconnection();
}

void Client::on_errorOccurred(QAbstractSocket::SocketError socketError)
{
    qDebug() << "connection error\n";
    emit clientError(socket->errorString());
    if (socket->errorString() == "Connection refused")
    {
        delete socket;
        socket = nullptr;
    }
}

void Client::sendMessage(const QString& message)
{
    socket->write(message.toUtf8());
}

