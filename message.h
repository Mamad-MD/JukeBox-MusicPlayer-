#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

enum class MessageType {
    Info,
    Warning,
    Critical
};


class Message
{
public:
    static void display(const MessageType, QString title, QString description);
};

#endif // MESSAGE_H
