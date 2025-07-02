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
<<<<<<< HEAD:MusicRoom/musicroom.h
#include "MusicPlayer/musicplayer.h"
#include <QList>
#include <QEventLoop>
=======
#include <QVBoxLayout>
#include "visualizerwidget.h"

>>>>>>> origin/echolaizer:musicroom.h

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
    void on_PushButton_Shuffle_clicked();
    void on_PushButton_Repeat_clicked();

    void loadFolderSelection();
    void on_musicPlayed();
    void on_musicPaused();
    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void on_metaDataChanged();
    void on_PushButton_SwitchView_clicked();

    void on_Slider_Volume_sliderReleased();
    void on_Slider_Volume_valueChanged(int value);
    void on_Slider_MusicSlider_sliderReleased();

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
    int currentlyPlayingIndex;
    qint64 currentlyPlayingDuration;
    bool shuffleOn;
    RepeatType repeatType;
	
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

    void playNext();
    void playPrev();
    void playThisIndex(int index);
    void playRandomIndex();

    QString formatTime(qint64 pos);

    VisualizerWidget* visualizer;

    bool isShowingCover = true;
};

#endif // MUSICROOM_H
