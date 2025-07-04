#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include "../../Network/Command/command.h"
#include <QFile>
#include <QDir>

class JoinWindow;
class MusicRoom;

class Client : public QObject {
    Q_OBJECT

public:
    static Client* getInstance(const QString& username);
    static void deleteInstance();
    void connectToHost(const int& port);
    void disconnect();
    void sendCommand(Command& command);

signals:
    void connectedToServer();
    void receivedRoomName(const QString& roomName);
    void disconnection();
    void dataReceived(const QByteArray& data);
    void clientObjectDeleted();
    void clientError(const QString& message);
    void goToMusicRoom();
    void clientNamesReceived(const QString& names);
    void newMessageReceived(const QString& username, const QString& msg);
    void signal_sayWhetherIHaveThisTrack(QString& track);
    void playTheTrack();
    void pauseTheTrack();

private slots:
    void connected();
    void readData();
    void disconnected();
    void on_errorOccurred(QAbstractSocket::SocketError socketError);

private:
    QString username;
    QString lastReceivedMessage;
    QTcpSocket* socket;
    bool hasReceivedRoomName;

    QFile* file;

    QString folderPath;
    QString currentTrackName;

    void cleanupSocket();
    static Client* instance;
    Client(const QString& username, QObject* parent = nullptr);
    ~Client();
    Client(const Client&) = delete;
    Client(const Client&&) = delete; 

    friend class JoinWindow;
    friend class MusicRoom;
};

#endif // CLIENT_H
