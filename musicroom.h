#ifndef MUSICROOM_H
#define MUSICROOM_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QStringListModel>
#include <QFileInfoList>
#include <QDir>
#include <QFileDialog>
#include <QMediaPlayer>
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
    int findIndexFromPath(const QString& path);
    bool isPlaying;

    QStringList musicPathsFromFolder;

    void iterateItemsInTree(QTreeWidgetItem* topItem);
    void showFolderTracks();
    void play(const QString& filePath = "");
    void pause();
    void changeActiveTrackInListView(int index);
};

#endif // MUSICROOM_H
