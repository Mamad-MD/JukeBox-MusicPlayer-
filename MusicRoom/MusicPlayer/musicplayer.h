#ifndef PLAYER_H
#define PLAYER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QWidget>
#include "../AudioTrack/audiotrack.h"

class MusicRoom;

enum class RepeatType {
    No_Repeat = 0,
    Repeat_Track,
    Repeat_List
};

class MusicPlayer : public QObject {
private:
    Q_OBJECT

    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    bool isPlaying;
    
    AudioTrack* currentTrack;
    friend class MusicRoom;

    // Singleton
    MusicPlayer(QWidget* parent);
    MusicPlayer(const MusicPlayer&) = delete;
    MusicPlayer(const MusicPlayer&&) = delete;
    ~MusicPlayer();
public:
    static MusicPlayer* instance;
    static MusicPlayer* getInstance(QWidget* parent);
    void play();
    void pause();
    void setAudioTrack(AudioTrack* audiotrack);
    void setVolume(const double& vol);
    void clearAudioTrack();

signals:
    void played();
    void paused();
    void audioCleared();
    void volumeChanged(float vol);
};


#endif
