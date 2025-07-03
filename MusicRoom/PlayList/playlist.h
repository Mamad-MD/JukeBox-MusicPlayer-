#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>
#include <QList>
#include "../AudioTrack/audiotrack.h"

class MusicRoom;

class PlayList
{
public:
    static AudioTrack* findTrackInListByName(QList<AudioTrack*>&, const QString& name);
    static AudioTrack* findTrackInListByName(QList<AudioTrack>&, const QString& name);
        PlayList() = default;
    PlayList(const QString& name);
    bool addTrack(AudioTrack& track);
    bool removeTrackByName(const QString& trackName);
    bool removeTrack(AudioTrack& track);
    QString name;
    QList<AudioTrack> tracks;
private:

    friend class MusicRoom;
};

#endif // PLAYLIST_H
