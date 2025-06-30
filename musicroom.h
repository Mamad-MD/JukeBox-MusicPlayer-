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
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QString currentlyPlayingPath;
    int currentlyPlayingIndex;
    qint64 currentlyPlayingDuration;
	
    int findIndexFromPath(const QString& path);
    bool isPlaying;

    QStringList musicPathsFromFolder;

    void iterateItemsInTree(QTreeWidgetItem* topItem);
    void showFolderTracks();
    void play(const QString& filePath = "");
    void pause();
    void changeActiveTrackInListView(int index);
    QString formatTime(qint64 pos);
};

#endif // MUSICROOM_H
