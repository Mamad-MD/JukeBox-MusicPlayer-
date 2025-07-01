#include "playlist.h"

PlayList::PlayList(const QString& name) 
{
    this->name = name;
}

void PlayList::addTrack(AudioTrack* track)
{
    tracks.append(track);
}

bool PlayList::removeTrackByName(const QString& trackName)
{
    for (auto& track : tracks)
        if (track->name == trackName)
            return tracks.removeOne(track);
    
    return false;
}

bool PlayList::removeTrackByPointer(AudioTrack* track)
{
    return tracks.removeOne(track);
}

AudioTrack* PlayList::findTrackInListByName(QList<AudioTrack*>& list, const QString& name)
{
    for (auto& track : list)
        if (track->name == name)
            return track;
    return nullptr;
}

AudioTrack* PlayList::findTrackInListByName(QList<AudioTrack>& list, const QString& name)
{
    for (auto& track : list)
        if (track.name == name)
            return &track;
    return nullptr;
}

