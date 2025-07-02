#include "audiotrack.h"

AudioTrack::AudioTrack(const QString& name, const QString& path):
name(name), filePath(path){}

QString AudioTrack::getName() const
{
    return name;
}
QString AudioTrack::getPath() const
{
    return filePath;
}

bool AudioTrack::operator==(const AudioTrack& track) const
{
    return name == track.getName() && filePath == track.getPath();
}

