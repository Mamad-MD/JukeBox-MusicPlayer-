#include "playlist.h"

PlayList::PlayList(const QString& name) 
{
    this->name = name;
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

bool PlayList::addTrack(AudioTrack& trackToBeAdded)
{
    for (auto& track : tracks)
        if (track.name == trackToBeAdded.name)
            return false;

    tracks.append(trackToBeAdded);
    return true;
}

bool PlayList::removeTrackByName(const QString& trackName)
{
    for (auto& track : tracks)
        if (track.name == trackName)
            return tracks.removeOne(track);

    return false;
}

bool PlayList::removeTrack(AudioTrack& track)
{
    return tracks.removeOne(track);
}

