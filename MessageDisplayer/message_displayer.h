#ifndef MESSAGE_DISPLAYER_H
#define MESSAGE_DISPLAYER_H

#include <QString>

enum class MessageType {
    Info,
    Warning,
    Critical
};


class MessageDisplayer
{
public:
    static void display(const MessageType, QString title, QString description);
};

#endif // MESSAGE_DISPLAYER_H
