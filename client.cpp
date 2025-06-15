#include "client.h"

client::client(QObject *parent): QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &connected);
    connect(socket, &QTcpSocket::readyRead, this, &readData);

    int port = 6000;
    socket->connectToHost(QHostAddress::Any, port);
}

void client::connected()
{

}
