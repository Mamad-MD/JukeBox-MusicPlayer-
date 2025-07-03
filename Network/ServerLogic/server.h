#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include "../../user.h"
#include "../Command/command.h"
#include <QFile>

class MusicRoom;

class Server : public QObject {
    Q_OBJECT

public:
    static Server* getInstance(const QString& roomName);
    static void deleteInstance();
    int getClientCount() const;
    QList<User>& getClientList();
    QString getRoomName() const;
    void broadcastMessage(Command& command, const QTcpSocket* excludedClientSocket = nullptr);
    void sendTrackToClient(QTcpSocket* socket, const QString& filePath);
    void sendMessageToClient(QTcpSocket* client, const QString message);
    void start(const int& port);
    void stop();
    QString getUsernamesAsQString();

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
    void clientsAllJoined();
    void allSetTheirFolders();
    void notHaveTheTrack(QTcpSocket* sender);
    void allHaveTheTrack();
    
private slots:
    void newConnection();
    void readData();
    void clientDisconnected();
    

private:
    QString roomName;
    QTcpServer *TCPServer;
    QList<User> clients;
    QList<User*> clientsInMusicRoom;
    QList<User*> clientsWhoSetTheirFolder;
    QList<User*> clientsWhoHaveTheTrack;

    QString currentTrackName;
    QString currentTrackPath;

    void addClient(User client);
    void removeClientBySocket(QTcpSocket* socket);
    void removeClientByUsername(const QString& username);
    User* findClientBySocket(const QTcpSocket* socket);
    static bool deletingInProcess;
    
    static Server* instance;
    Server(const QString& roomName, QObject* parent = nullptr);
    ~Server();
    Server(const Server&) = delete;
    // Server& operator=(const Server&) = delete;
    Server(const Server&&) = delete;
    // Server& operator=(const Server&&) = delete;

    friend class MusicRoom;
};

#endif // SERVER_H
