#include "musicroom.h"
#include "ui_musicroom.h"
#include "../MessageDisplayer/message_displayer.h"
#include "../Random/RandomGenerator.h"

// class MusicRoom;

MusicPlayer* MusicPlayer::instance = nullptr;

MusicRoom::MusicRoom(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicRoom), hasSetFolder(false), hasAQueue(false), model(nullptr), shuffleOn(false),
    repeatType(RepeatType::No_Repeat)
{
    ui->setupUi(this);
    ui->Label_AlbumCover->setScaledContents(true);
    for (int i = 0; i < ui->TreeWidget_Category->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* topItem = ui->TreeWidget_Category->topLevelItem(i);
        categoryItems[i] = topItem;
        // iterateItemsInTree(topItem);
    }
    
    musicPlayer = MusicPlayer::getInstance(this);
    
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

QString MusicRoom::formatTime(qint64 ms)
{
    int seconds = ms / 1000;
    int minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

int MusicRoom::findIndexFromPath(const QString& path)
{
    for (int i = 0; i < tracksFromFolder.size(); i++)
    {
        if (path == tracksFromFolder[i].filePath)
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

    // musicPathsFromFolder.clear();
    tracksFromFolder.clear();
    tracksInListView.clear();
    
    for (const QFileInfo &fileInfo : fileList) 
    {
        AudioTrack track(fileInfo.fileName(), fileInfo.absoluteFilePath());
        tracksFromFolder.append(track);

        fileNames << fileInfo.fileName();
    }

    for (auto& track : tracksFromFolder)
        tracksInListView.append(&track);

    // =================================================
    // I Want you save these tracks somewhere in a file


    

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

void MusicRoom::showPlayListTracks(const QString& playlistName)
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Show Playlist " + playlistName);
}

void MusicRoom::connectPlayerSignalsToUISlots()
{
    connect(musicPlayer->player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
    qDebug() << "Error:" << error << errorString;
    });


    connect(musicPlayer, &MusicPlayer::played, this, &MusicRoom::on_musicPlayed);
    connect(musicPlayer, &MusicPlayer::paused, this, &MusicRoom::on_musicPaused);
    
    connect(musicPlayer->player, &QMediaPlayer::positionChanged, this, &MusicRoom::on_positionChanged);
    connect(musicPlayer->player, &QMediaPlayer::durationChanged, this, &MusicRoom::on_durationChanged);
    connect(musicPlayer->player, &QMediaPlayer::metaDataChanged, this, &MusicRoom::on_metaDataChanged);
}

void MusicRoom::clearTracksListView()
{
    if (!model)
        return;
    musicPlayer->pause();
    musicPlayer->clearAudioTrack();
    ui->Label_Timer->setText("");
    tracksInListView.clear();
    model->setStringList(QStringList());  // Clears the list
    ui->ListView_AudioTracks->setModel(model);
}

void MusicRoom::playNext()
{
    if (shuffleOn)
    {
        playRandomIndex();
        return;
    }
    
    if (currentlyPlayingIndex == tracksInListView.size() - 1)
    {
        currentlyPlayingIndex = 0;
        playThisIndex(currentlyPlayingIndex);
    }
    else
    {
        currentlyPlayingIndex++;
        playThisIndex(currentlyPlayingIndex);
    }
}

void MusicRoom::playPrev()
{
    if (shuffleOn)
    {
        playRandomIndex();
        return;
    }

    if (currentlyPlayingIndex == 0)
    {
        currentlyPlayingIndex = tracksInListView.size() - 1;
        playThisIndex(currentlyPlayingIndex);
    }
    else
    {
        currentlyPlayingIndex--;
        playThisIndex(currentlyPlayingIndex);
    }
}

void MusicRoom::playThisIndex(int index)
{
    musicPlayer->setAudioTrack(tracksInListView[currentlyPlayingIndex]);
    musicPlayer->play();
    changeActiveTrackInListView(currentlyPlayingIndex);
}

void MusicRoom::playRandomIndex()
{
    if (tracksInListView.size() == 1)
    {
        if (repeatType != RepeatType::No_Repeat)
            playThisIndex(currentlyPlayingIndex);
        return;
    }

    int randomIndex = generateRandom(0, tracksInListView.size() - 1);
    while (randomIndex == currentlyPlayingIndex)
        randomIndex = generateRandom(0, tracksInListView.size() - 1);
    currentlyPlayingIndex = randomIndex;
    playThisIndex(currentlyPlayingIndex);
}
