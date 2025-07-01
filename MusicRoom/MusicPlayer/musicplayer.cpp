#include "musicplayer.h"

MusicPlayer* MusicPlayer::getInstance(QWidget* parent)
{
    if (!instance)
        instance = new MusicPlayer(parent);
    return instance;
}

MusicPlayer::MusicPlayer(QWidget* parent)
{
    player = new QMediaPlayer(parent);
    audioOutput = new QAudioOutput(parent);
    player->setAudioOutput(audioOutput);
    isPlaying = false;
}

MusicPlayer::~MusicPlayer()
{
    player->deleteLater();
    audioOutput->deleteLater();
}

void MusicPlayer::play()
{
    if (currentTrack->filePath == "")
    {
        player->play(); //resume
        isPlaying = true;
        emit played();
        return;
    }

    // player->stop();
    player->setSource(QUrl::fromLocalFile(currentTrack->filePath));
    audioOutput->setVolume(0.5);
    player->play();
    isPlaying = true;

    emit played();
}

void MusicPlayer::pause()
{
    player->pause();
    isPlaying = false;
    emit paused();
}

void MusicPlayer::setAudioTrack(const AudioTrack* audiotrack)
{
    currentTrack = audiotrack;
}
