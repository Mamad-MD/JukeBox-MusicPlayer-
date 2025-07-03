#include "UserDataFileManager.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDebug>

QDataStream& operator<<(QDataStream& out, const AudioTrack& track) {
    out << track.getName() << track.getPath();
    return out;
}

QDataStream& operator>>(QDataStream& in, AudioTrack& track) {
    QString name, path;
    in >> name >> path;
    track = AudioTrack(name, path);
    return in;
}

QDataStream& operator<<(QDataStream& out, const PlayList& playlist) {
    out << playlist.name << playlist.tracks;
    return out;
}

QDataStream& operator>>(QDataStream& in, PlayList& playlist) {
    in >> playlist.name >> playlist.tracks;
    return in;
}

bool UserDataFileManager::saveUserData(const QString& username,
                                       const QList<PlayList>& playlists,
                                       const QList<AudioTrack>& favorites,
                                       const QList<AudioTrack>& queue)
{
    QDir().mkdir("user_data");
    QFile file("user_data/" + username + ".dat");
    if (!file.open(QIODevice::WriteOnly)) return false;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_6);
    out << playlists << favorites << queue;
    file.close();
    return true;
}

bool UserDataFileManager::loadUserData(const QString& username,
                                       QList<PlayList>& playlists,
                                       QList<AudioTrack>& favorites,
                                       QList<AudioTrack>& queue)
{
    QFile file("user_data/" + username + ".dat");
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly)) return false;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_6);
    in >> playlists >> favorites >> queue;
    file.close();
    return true;
}
