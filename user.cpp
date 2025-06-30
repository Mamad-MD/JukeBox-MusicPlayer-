#pragma once

#include "user.h"

QString User::getUsername()
{
    return username;
}

QDateTime User::getJoinTime()
{
    return joinTime;
}

QTcpSocket* User::getSocket()
{
    return socket;
}

bool User::operator==(const User& user) const
{
    return username == user.username &&
            socket == user.socket &&
            joinTime == user.joinTime;
}
