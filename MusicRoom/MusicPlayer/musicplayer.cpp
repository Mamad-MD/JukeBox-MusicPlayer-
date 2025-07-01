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

void MusicPlayer::setAudioTrack(AudioTrack* audiotrack)
{
    currentTrack = audiotrack;
    player->setSource(QUrl::fromLocalFile(audiotrack->filePath));
}

void MusicPlayer::setVolume(const double& vol)
{
    if (vol > 1 || vol < 0)
        audioOutput->setVolume(1);
    else
        audioOutput->setVolume(vol);
    emit volumeChanged(audioOutput->volume());
}

void MusicPlayer::clearAudioTrack()
{
    pause();
    currentTrack = nullptr;
    player->setSource(QUrl());
    emit audioCleared();
}
