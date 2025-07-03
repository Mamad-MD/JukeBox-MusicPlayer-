#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include <QString>

class MusicRoom;
class MusicPlayer;
class PlayList;

class AudioTrack {
public:
    AudioTrack() = default;
    AudioTrack(const QString& name, const QString& path);
    bool operator==(const AudioTrack& track) const;
    QString getName() const;
    QString getPath() const;
    QString name;
    QString filePath;
private:


    friend class MusicPlayer;
    friend class MusicRoom;
    friend class PlayList;
};

#endif
