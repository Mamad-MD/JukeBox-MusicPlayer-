#include "command.h"
#include <QIODevice>
#include <QDebug>

Command::Command(CommandType commandType, QString username, QString content):
commandType(commandType), username(username), content(content) {}

QDataStream& operator<<(QDataStream& out, const Command& command)
{
    out << static_cast<int>(command.commandType) << command.username << command.content;
    return out;
}

QDataStream& operator>>(QDataStream& in, Command& command)
{
    int index;
    in >> index >> command.username >> command.content;
    command.commandType = static_cast<CommandType>(index);
    return in;
}

Command byteArrayToCommand(QByteArray* byteArray)
{
    QDataStream in(byteArray, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    Command command;
    in >> command;

    return command;
}

QByteArray commandToByteArray(Command* command)
{
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << *command;
    return byteArray;
}
