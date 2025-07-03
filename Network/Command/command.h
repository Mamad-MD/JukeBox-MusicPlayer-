#ifndef COMMAND_H
#define COMMAND_H

#include "commandtype.h"
#include <QString>
#include <QDataStream>

class Server;
class Client;
class MusicRoom;

class Command
{
public:
    Command() = default;
    Command(CommandType commandType, QString username, QString content);

    friend QDataStream& operator<<(QDataStream& out, const Command& command);
    friend QDataStream& operator>>(QDataStream& om, Command& command);
    friend Command byteArrayToCommand(QByteArray* byteArray);
    friend QByteArray commandToByteArray(Command* command);
    friend class Server;
    friend class Client;
    friend class MusicRoom;
private:
    CommandType commandType;
    QString username;
    QString content;
};


Command byteArrayToCommand(QByteArray* byteArray);
QByteArray commandToByteArray(Command* command);


#endif // COMMAND_H
