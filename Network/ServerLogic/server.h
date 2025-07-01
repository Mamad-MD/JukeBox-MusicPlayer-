#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include "user.h"
#include "command.h"

class Server : public QObject {
    Q_OBJECT

public:
    static Server* getInstance(const QString& roomName);
    static void deleteInstance();
    int getClientCount() const;
    QList<User>& getClientList();
    QString getRoomName() const;
    void broadcastMessage(Command& command, const QTcpSocket* excludedClientSocket = nullptr);
    void sendMessageToClient(QTcpSocket* client, const QString message);
    void start(const int& port);
    void stop();

signals:
    void serverStarted(int port);
    void serverStopped();
    void serverDeleted();
    void serverError(const QString& errorMessage);
    void clientConnected(const QString& username, int clientsCount);
    void clientDisconnection(const QString& username, int clientsCount);
    void dataReceived(const QString& username, const QByteArray& message);
    void clientConnectedToMainServer();
    void messageReceived(const QString& username, const QString& content);

private slots:
    void newConnection();
    void readData();
    void clientDisconnected();
    

private:
    QString roomName;
    QTcpServer *TCPServer;
    QList<User> clients;

    void addClient(User client);
    void removeClientBySocket(QTcpSocket* socket);
    void removeClientByUsername(const QString& username);
    QString findClientBySocket(const QTcpSocket* socket);
    static bool deletingInProcess;
    
    static Server* instance;
    Server(const QString& roomName, QObject* parent = nullptr);
    ~Server();
    Server(const Server&) = delete;
    // Server& operator=(const Server&) = delete;
    Server(const Server&&) = delete;
    // Server& operator=(const Server&&) = delete;
};

#endif // SERVER_H
