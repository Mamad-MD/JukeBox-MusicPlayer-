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
    PlayList(const QString& name);
    void addTrack(AudioTrack* track);
    bool removeTrackByName(const QString& trackName);
    bool removeTrackByPointer(AudioTrack* track);
private:
    QString name;
    QList<AudioTrack*> tracks;
    friend class MusicRoom;
};

#endif // PLAYLIST_H
