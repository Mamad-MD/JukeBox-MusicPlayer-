#ifndef MUSICROOM_H
#define MUSICROOM_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QStringListModel>
#include <QFileInfoList>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include <QList>
#include <QEventLoop>
#include <QVBoxLayout>
#include "MusicPlayer/musicplayer.h"
#include "Visualizer/visualizerwidget.h"
#include "PlayList/playlist.h"
#include "../Network/ServerLogic/server.h"
#include "../Network/ClientLogic/client.h"

namespace Ui {
class MusicRoom;
}

enum class ViewMode{
    None = 0,
    Folder,
    PlayList,
    Queue
};

enum class NetworkMode {
    Offline = 0,
    Client,
    Server
};

class MusicRoom : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicRoom(NetworkMode networkMode, Server* server, Client* client,QWidget *parent = nullptr);
    ~MusicRoom();

private slots:
    void on_TreeWidget_Category_itemActivated(QTreeWidgetItem *item, int column);
    void on_ListView_AudioTracks_doubleClicked(const QModelIndex &index);
    void on_PushButton_Browse_clicked();
    void on_PushButton_PlayPause_clicked();
    void on_PushButton_Next_clicked();
    void on_PushButton_Prev_clicked();
    void on_PushButton_Shuffle_clicked();
    void on_PushButton_Repeat_clicked();

    void loadFolderSelection();
    void on_musicPlayed();
    void on_musicPaused();
    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void on_metaDataChanged();
    void on_PushButton_SwitchView_clicked();

    void on_Slider_Volume_valueChanged(int value);
    void on_Slider_MusicSlider_sliderReleased();
    void on_PushButton_CreatePlayList_clicked();
    void on_PushButton_AddTrackToPlayList_clicked();
    void on_PushButton_AddTrackToQueue_clicked();


    // Client Slots:
    void on_clientNamesReceived(const QString& names);

    // Server Slots:
    void on_clientsAllJoined();

private:
    Ui::MusicRoom *ui;
    
    QTreeWidgetItem* categoryItems[3];
    bool hasSetFolder;
    bool hasAQueue;

    QStringListModel* modelForTracksListView;
    QStringListModel* modelForPlaylistsListView;
    
    MusicPlayer* musicPlayer;
    QList<AudioTrack> tracksFromFolder;
    QList<AudioTrack> tracksFromQueue;
    QList<PlayList> playlists;
    QList<AudioTrack*> tracksInListView; // it doesn't care what library
                                         // you're listening to. it just stores
                                         // a pointer of ListView tracks
    int currentlyPlayingIndex;
    qint64 currentlyPlayingDuration;
    QString activePlaylist;
    bool shuffleOn;
    RepeatType repeatType;
    ViewMode viewMode;
	
    int findIndexFromPath(const QString& path);


    void showFolderTracks();
    void showQueueTracks();
    void showPlayListTracks(const QString& playlistName);

    
    void iterateItemsInTree(QTreeWidgetItem* topItem);
    void changeActiveTrackInListView(int index);

    QString formatTime(qint64 ms);
    void connectPlayerSignalsToUISlots();
    void clearTracksListView();
    void clearPlaylistsListView();
    void addTracksToListView(QList<AudioTrack>& tracks);
    bool addTrackToQueue(AudioTrack& track);
    void reloadPlaylistsInListView();
    void reloadPlaylistsInTree();
    PlayList* findPlaylistByName(const QString& name);

    void playNext();
    void playPrev();
    void playThisIndex(int index);
    void playRandomIndex();

    VisualizerWidget* visualizer;

    bool isShowingCover = true;


    // Online Stuff
    Server* server;
    Client* client;
    NetworkMode networkMode;

    void connectClientSignalsToUI();
    void connectServerSignalsToUI();

    QStringListModel* modelForClientListView;
    
    void addClientsToListView(const QString& names);
    void clearClientsList();
};

#endif // MUSICROOM_H
