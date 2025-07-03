#ifndef USERDATAFILEMANAGER_H
#define USERDATAFILEMANAGER_H

#include "../MusicRoom/PlayList/playlist.h"
#include "../MusicRoom/AudioTrack/audiotrack.h"
#include <QString>
#include <QList>

class UserDataFileManager {
public:
    static bool saveUserData(const QString& username,
                             const QList<PlayList>& playlists,
                             const QList<AudioTrack>& favorites,
                             const QList<AudioTrack>& queue);

    static bool loadUserData(const QString& username,
                             QList<PlayList>& playlists,
                             QList<AudioTrack>& favorites,
                             QList<AudioTrack>& queue);
};

#endif // USERDATAFILEMANAGER_H
