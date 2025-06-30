#ifndef COMMANDTYPE_H
#define COMMANDTYPE_H

enum class CommandType {
    Unset = 0,
    Join_Request,
    Message,
    GoToMusicRoom,
    Play_Request,
    Pause_Request
};

#endif // COMMANDTYPE_H
