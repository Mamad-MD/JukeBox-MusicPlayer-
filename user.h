#ifndef USER_H
#define USER_H

#include <QString>
#include <QTcpSocket>
#include <QDateTime>
#include "server.h"

class User {
private:
    QString username;
    QTcpSocket* socket;
    QDateTime joinTime;
    friend class Server;

public:
    User(QTcpSocket* socket, const QString& username = "-1"): username(username), socket(socket),
        joinTime(QDateTime::currentDateTime()) {}
};


#endif // USER_H
