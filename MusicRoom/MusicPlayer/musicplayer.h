#ifndef PLAYER_H
#define PLAYER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QWidget>
#include "../AudioTrack/audiotrack.h"

class MusicRoom;

class MusicPlayer {
private:
    Q_OBJECT

    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    bool isPlaying;
    
    AudioTrack* currentTrack;
    friend class MusicRoom;

    // Singleton
    static MusicPlayer* instance;
    MusicPlayer(QWidget* parent);
    MusicPlayer(const MusicPlayer&) = delete;
    MusicPlayer(const MusicPlayer&&) = delete;
    ~MusicPlayer();
public:
    static MusicPlayer* getInstance();
    void play();
    void pause();
    void setAudioTrack(const AudioTrack* audiotrack);

private signals:
    void played();
    void paused();
    
};


#endif
