#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include "user.h"

class Server : public QObject {
    Q_OBJECT

public:
    Server(const QString& roomName, QObject* parent = nullptr);
    int getClientCount() const;
    QStringList getClientList() const;
    QString getRoomName() const;
    void broadcastMessage(const QString& message, const QTcpSocket* excludedClientSocket = nullptr);
    void sendMessageToClient(QTcpSocket* client, const QString message);
    

signals:
    void serverStarted(int port);
    void serverError(const QString& errorMessage);
    void clientConnected(const QString& username, int clientsCount);
    void clientDisconnection(const QString& username, int clientsCount);
    void dataReceived(const QString& username, const QByteArray& message);


private slots:
    void newConnection();  // Called when a client connects
    void readData();       // Called when data is available to read
    void clientDisconnected();
    

private:
    QString roomName;
    QTcpServer *TCPServer;

    QList<User> clients;
    void addClient(User client);
    void removeClientBySocket(QTcpSocket* socket);
    QString findClientBySocket(const QTcpSocket* socket);
};

#endif // SERVER_H
