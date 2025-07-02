#include "musicroom.h"
#include "ui_musicroom.h"
#include "../MessageDisplayer/message_displayer.h"
#include "../Random/RandomGenerator.h"

// class MusicRoom;

MusicPlayer* MusicPlayer::instance = nullptr;

MusicRoom::MusicRoom(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicRoom), hasSetFolder(false), hasAQueue(false), modelForTracksListView(nullptr), modelForPlaylistsListView(nullptr), shuffleOn(false), currentlyPlayingIndex(0)
    ,repeatType(RepeatType::No_Repeat), viewMode(ViewMode::None)
{
    ui->setupUi(this);
    ui->Label_AlbumCover->setScaledContents(true);
    ui->Label_AlbumCover->setVisible(true);
    for (int i = 0; i < ui->TreeWidget_Category->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* topItem = ui->TreeWidget_Category->topLevelItem(i);
        categoryItems[i] = topItem;
        // iterateItemsInTree(topItem);
    }
    
    musicPlayer = MusicPlayer::getInstance(this);
    musicPlayer->setVolume((float)ui->Slider_Volume->value() / 100);

    // Visualizer
    visualizer = new VisualizerWidget(this);
    QWidget* visualizerPage = ui->Widget_MusicDisplay->widget(1);
    QVBoxLayout* visualizerLayout = qobject_cast<QVBoxLayout*>(visualizerPage->layout());
    if (!visualizerLayout) {
        visualizerLayout = new QVBoxLayout(visualizerPage);
        visualizerPage->setLayout(visualizerLayout);
    }

    visualizerLayout->addWidget(visualizer);
    visualizerLayout->setContentsMargins(0, 0, 0, 0);

    isShowingCover = true;
    ui->Widget_MusicDisplay->setCurrentIndex(0); // نمایش اولیه کاور
    ui->Label_AlbumCover->show();                // حتما کاور نمایش داده بشه
    visualizer->hide();
    ui->PushButton_SwitchView->setText("Visualizer");


    // =======================================
    // I wanna read user's playlists and folder here and update the listviews.
    //========================================


    // Load Playlists names
    reloadPlaylistsInTree();



    


    //  connect(ui->PushButton_SwitchView, &QPushButton::clicked,this, &MusicRoom::on_PushButton_SwitchView_clicked);
    // connect(ui->Slider_MusicSlider, &QSlider::sliderReleased, this, &MusicRoom::on_sliderReleased);
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
    if (viewMode != ViewMode::Folder)
        return;
    
    QString dirPath = ui->LineEdit_Path->text();

    QDir dir(dirPath);
    QStringList filters;
    filters << "*.mp3" << "*.wav";

    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);

    // musicPathsFromFolder.clear();
    tracksFromFolder.clear();
    
    for (const QFileInfo &fileInfo : fileList) 
    {
        AudioTrack track(fileInfo.fileName(), fileInfo.absoluteFilePath());
        tracksFromFolder.append(track);
    }


    addTracksToListView(tracksFromFolder);


    // =================================================
    // I Want you save these tracks somewhere in a file

}

void MusicRoom::showQueueTracks()
{
    if (viewMode != ViewMode::Queue)
        return;
    // MessageDisplayer::display(MessageType::Info, "Notice", "Show Queue Tracks");
    addTracksToListView(tracksFromQueue);
    
}

void MusicRoom::showPlayListTracks(const QString& playlistName)
{
    if (viewMode != ViewMode::PlayList)
        return;
    // MessageDisplayer::display(MessageType::Info, "Notice", "Show Playlist " + playlistName);
    for (auto& playlist : playlists)
    {
        if (playlist.name == playlistName)
            addTracksToListView(playlist.tracks);
    }
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
    if (!modelForTracksListView)
        return;
    musicPlayer->pause();
    musicPlayer->clearAudioTrack();
    ui->Label_Timer->setText("");
    tracksInListView.clear();
    modelForTracksListView->setStringList(QStringList());  // Clears the list
    ui->ListView_AudioTracks->setModel(modelForTracksListView);
}

void MusicRoom::playNext()
{
    if (tracksInListView.size() == 0)
        return;
        
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
    if (tracksInListView.size() == 0)
        return;
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
    if (tracksInListView.size() == 0)
        return;
    musicPlayer->setAudioTrack(tracksInListView[currentlyPlayingIndex]);
    musicPlayer->play();
    changeActiveTrackInListView(currentlyPlayingIndex);
}

void MusicRoom::playRandomIndex()
{
    if (tracksInListView.size() == 0)
        return;
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

void MusicRoom::addTracksToListView(QList<AudioTrack>& tracks)
{
    void clearTracksListView();
    QStringList trackNames;
    tracksInListView.clear();
    for (auto& track : tracks)
    {
        trackNames << track.name;
        tracksInListView.append(&track);
    }

    if (!modelForTracksListView)
        modelForTracksListView = new QStringListModel(this);
    
    modelForTracksListView->setStringList(trackNames);
    ui->ListView_AudioTracks->setModel(modelForTracksListView);
}

void MusicRoom::reloadPlaylistsInListView()
{
    clearPlaylistsListView();
    QStringList playlistNames;
    for (auto& playlist : playlists)
        playlistNames << playlist.name;
        
    if (!modelForPlaylistsListView)
        modelForPlaylistsListView = new QStringListModel(this);
        
    modelForPlaylistsListView->setStringList(playlistNames);
    ui->ListView_PlayLists->setModel(modelForPlaylistsListView);

}

void MusicRoom::reloadPlaylistsInTree()
{
    QTreeWidget* tree = ui->TreeWidget_Category;
    QTreeWidgetItem* playlistsBranch = tree->topLevelItem(1);

    // First clear it
    for (int i = 0; i < playlistsBranch->childCount(); i++)
        playlistsBranch->takeChild(i);

    for (auto& playlist : playlists)
    {
        auto playlistBranch = new QTreeWidgetItem();
        playlistBranch->setText(0, playlist.name);
        playlistsBranch->addChild(playlistBranch);
    }
}

PlayList* MusicRoom::findPlaylistByName(const QString& name)
{
    for (auto& playlist : playlists)
    {
        if (playlist.name == name)
            return &playlist;
    }

    return nullptr;
}

void MusicRoom::clearPlaylistsListView()
{
    if (!modelForPlaylistsListView)
        return;
    modelForPlaylistsListView->setStringList(QStringList());
    ui->ListView_PlayLists->setModel(modelForPlaylistsListView);
}

bool MusicRoom::addTrackToQueue(AudioTrack& trackToBeAdded)
{
    for (auto& track : tracksFromQueue)
        if (track.name == trackToBeAdded.name)
            return false;

    tracksFromQueue.append(trackToBeAdded);
    return true;
}
