#ifndef MUSICROOM_H
#define MUSICROOM_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QStringListModel>
#include <QFileInfoList>
#include <QDir>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include "MusicPlayer/musicplayer.h"
#include <QList>
#include <QEventLoop>

namespace Ui {
class MusicRoom;
}

class MusicRoom : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicRoom(QWidget *parent = nullptr);
    ~MusicRoom();

private slots:
    void on_TreeWidget_Category_itemActivated(QTreeWidgetItem *item, int column);
    void on_ListView_AudioTracks_doubleClicked(const QModelIndex &index);
    void on_PushButton_Browse_clicked();
    void on_PushButton_PlayPause_clicked();
    void on_PushButton_Next_clicked();
    void on_PushButton_Prev_clicked();

    void loadFolderSelection();
    void on_musicPlayed();
    void on_musicPaused();
    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void on_sliderReleased();
    void on_metaDataChanged();

private:
    Ui::MusicRoom *ui;
    
    QTreeWidgetItem* categoryItems[3];
    bool hasSetFolder;
    bool hasAQueue;

    QStringListModel* model;
    
    MusicPlayer* musicPlayer;
    QList<AudioTrack> tracksFromFolder;
    QList<AudioTrack*> tracksInListView; // it doesn't care what library
                                         // you're listening to. it just stores
                                         // a pointer of ListView tracks
    
    // QString currentlyPlayingPath;
    int currentlyPlayingIndex;
    qint64 currentlyPlayingDuration;
	
    int findIndexFromPath(const QString& path);

    // QStringList musicPathsFromFolder;

    void showFolderTracks();
    void showQueueTracks();
    void showPlayListTracks(const QString& playlistName);
    // void clearAudioTracks

    
    void iterateItemsInTree(QTreeWidgetItem* topItem);
    // void play(const QString& filePath = "");
    // void pause();
    void changeActiveTrackInListView(int index);
    QString formatTime(qint64 ms);
    void connectPlayerSignalsToUISlots();
    void clearTracksListView();
};

#endif // MUSICROOM_H
