#ifndef COMMANDTYPE_H
#define COMMANDTYPE_H

enum class CommandType {
    Unset = 0,
    RoomName_Sending,
    Join_Request,
    Message,
    GoToMusicRoom,
    JoinedMusicRoom,
    ClientNames_Sending,
    Play_Request,
    Pause_Request,
    Play_New,
    Request_For_ClientNames,
    Response_For_ClientNames,
    SetMyFolder,
    WannaPlayThisTrack,
    ThisTrackStatusOnMyPC,
    SendingDone
};

#endif // COMMANDTYPE_H
