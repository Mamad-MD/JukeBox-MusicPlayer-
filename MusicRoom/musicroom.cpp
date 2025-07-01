#include "musicroom.h"
#include "ui_musicroom.h"
#include "../MessageDisplayer/message_displayer.h"

// class MusicRoom;

MusicRoom::MusicRoom(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicRoom), hasSetFolder(false), hasAQueue(false), model(nullptr)
{
    ui->setupUi(this);
    ui->Label_AlbumCover->setScaledContents(true);
    for (int i = 0; i < ui->TreeWidget_Category->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* topItem = ui->TreeWidget_Category->topLevelItem(i);
        categoryItems[i] = topItem;
        // iterateItemsInTree(topItem);
    }
    
    musicPlayer = MusicPlayer::getInstance();
    
    connect(ui->Slider_MusicSlider, &QSlider::sliderReleased, this, &MusicRoom::on_sliderReleased);
    connectPlayerSignalsToUISlots();
}

void MusicRoom::iterateItemsInTree(QTreeWidgetItem* item)
{
    if (!item) return;

    // Do something with the item
    qDebug() << "Item text:" << item->text(0);

    // Recurse into children
    for (int i = 0; i < item->childCount(); ++i)
        iterateItemsInTree(item->child(i));
}

MusicRoom::~MusicRoom()
{
    delete ui;
}

void MusicRoom::loadFolderSelection()
{
    
}

// void MusicRoom::play(const QString& filePath)
// {
//     if (filePath == "")
//     {
//         player->play(); //resume
//         isPlaying = true;
//         on_musicPlayed();
//         return;
//     }

//     //  extract music cover                                                       =============

//     // player->stop();
//     player->setSource(QUrl::fromLocalFile(filePath));
//     audioOutput->setVolume(0.5);
//     player->play();
//     isPlaying = true;
//     currentlyPlayingPath = filePath;
//     currentlyPlayingIndex = findIndexFromPath(filePath);
//     on_musicPlayed();
// }

// void MusicRoom::pause()
// {
//     player->pause();
//     isPlaying = false;
//     on_musicPaused();
// }

QString MusicRoom::formatTime(qint64 ms)
{
    int seconds = pos / 1000;
    int minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

int MusicRoom::findIndexFromPath(const QString& path)
{
    for (int i = 0; i < musicPathsFromFolder.size(); i++)
    {
        if (path == musicPathsFromFolder[i])
            return i;
    }

    return -1;
}

void MusicRoom::changeActiveTrackInListView(int index)
{
    ui->ListView_AudioTracks->selectionModel()->clearSelection();
    QModelIndex modelIndex = ui->ListView_AudioTracks->model()->index(index, 0);
    ui->ListView_AudioTracks->setCurrentIndex(modelIndex);
    ui->ListView_AudioTracks->selectionModel()->select(modelIndex, QItemSelectionModel::Select);
}

void MusicRoom::showFolderTracks()
{
    QString dirPath = ui->LineEdit_Path->text();

    QDir dir(dirPath);
    QStringList filters;
    filters << "*.mp3" << "*.wav";

    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
    QStringList fileNames;

    musicPathsFromFolder.clear();

    for (const QFileInfo &fileInfo : fileList) 
    {
        fileNames << fileInfo.fileName();
        musicPathsFromFolder << fileInfo.absoluteFilePath();
    }

    if (model)
    {
        delete model;
        model = nullptr;
    }

    model = new QStringListModel(this);

    model->setStringList(fileNames);
    ui->ListView_AudioTracks->setModel(model);
}

void MusicRoom::showQueueTracks()
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Show Queue Tracks");
}

void MusicRoom::showPlayListTracks(const QStirng& playlistName);
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Show Playlist " + playlistName);
}

void MusicRoom::connectPlayerSignalsToUISlots()
{
    connect(musicPlayer->player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
    qDebug() << "Error:" << error << errorString;
    });
    
    connect(musicPlayer->player, &QMediaPlayer::positionChanged, this, &MusicRoom::on_positionChanged);
    connect(musicPlayer->player, &QMediaPlayer::durationChanged, this, &MusicRoom::on_durationChanged);
    connect(musicPlayer->player, &QMediaPlayer::metaDataChanged, this, &MusicRoom::on_metaDataChanged);
}
