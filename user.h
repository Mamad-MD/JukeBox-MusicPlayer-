#ifndef USER_H
#define USER_H

#include <QString>
#include <QTcpSocket>
#include <QDateTime>

class MusicRoom;

class User {
private:
    QString username;
    QTcpSocket* socket;
    QDateTime joinTime;
    friend class Server;

public:
    User(QTcpSocket* socket, const QString& username = "-1"): username(username), socket(socket),
        joinTime(QDateTime::currentDateTime()) {}
    QString getUsername();
    QDateTime getJoinTime();
    QTcpSocket* getSocket();
    bool operator==(const User& user) const;
    friend class MusicRoom;
};


#endif // USER_H
