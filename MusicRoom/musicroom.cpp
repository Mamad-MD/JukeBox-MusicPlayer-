#include "musicroom.h"
#include "ui_musicroom.h"
#include "../MessageDisplayer/message_displayer.h"
#include "../Random/RandomGenerator.h"
#include "../Login/userdatafilemanager.h"
#include "../Login/authmanager.h"
// class MusicRoom;

MusicPlayer* MusicPlayer::instance = nullptr;

MusicRoom::MusicRoom(NetworkMode networkMode, Server* server, Client* client, QWidget *parent/*,const QString& username*/)
    : QMainWindow(parent) /*,currentUsername(username)*/
    , ui(new Ui::MusicRoom), hasSetFolder(false), hasAQueue(false), modelForTracksListView(nullptr), modelForPlaylistsListView(nullptr), shuffleOn(false), currentlyPlayingIndex(0)
    ,repeatType(RepeatType::No_Repeat), viewMode(ViewMode::None), networkMode(networkMode), server(server), client(client),
    modelForClientListView(nullptr), currentTrack(nullptr)
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


    //connect(ui->PushButton_AddTrackToFavorite, &QPushButton::clicked,this, &MusicRoom::on_PushButton_AddTrackToFavorite_clicked);


    // =======================================
    // I wanna read user's playlists and folder here and update the listviews.
    //========================================


    // Load Playlists names
    reloadPlaylistsInTree();


    QString username = Authmanager::getLoggedInUsername();
    UserDataFileManager::loadUserData(username, playlists, FaveriteTracks, tracksFromQueue);

    reloadPlaylistsInTree();


    //  connect(ui->PushButton_SwitchView, &QPushButton::clicked,this, &MusicRoom::on_PushButton_SwitchView_clicked);
    // connect(ui->Slider_MusicSlider, &QSlider::sliderReleased, this, &MusicRoom::on_sliderReleased);
    connectPlayerSignalsToUISlots();
    connectClientSignalsToUI();
    connectServerSignalsToUI();
    
    // This makes the window so nice and clean when online stuff is not needed.
    if (networkMode == NetworkMode::Offline)
    {
        ui->GroupBox_OnlineBox->setEnabled(false);
        this->resize(875, this->height());
        ui->GroupBox_OnlineBox->deleteLater();
        ui->GroupBox_OnlineBox = nullptr;
        
        return;
    }

    if (client)
    {
        disableClientUI();

        Command command(CommandType::JoinedMusicRoom, "", "");
        client->sendCommand(command);
    }


    QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);

    bool isPlaying = false;
}

void MusicRoom::connectClientSignalsToUI()
{
    connect(client, Client::clientNamesReceived, this, on_clientNamesReceived);
    connect(client, Client::newMessageReceived, this, on_newMessageReceived);
    connect(client, Client::signal_sayWhetherIHaveThisTrack, this, sayWhetherIHaveThisTrack);
    connect(client, Client::playTheTrack, this, on_playTheTrackOnline);
    connect(client, Client::pauseTheTrack, this, on_pauseTheTrackOnline);
}

void MusicRoom::connectServerSignalsToUI()
{
    connect(server, Server::clientsAllJoined, this, on_clientsAllJoined);
    connect(server, Server::clientDisconnection, this, on_clientDisconnected);
    connect(server, Server::messageReceived, this, on_messageReceived);
    connect(server, Server::allSetTheirFolders, this, on_allSetTheirFolders);
    connect(server, Server::notHaveTheTrack, this, on_notHaveTheTrack);
    connect(server, Server::allHaveTheTrack, this, on_allHaveTheTrack);
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
    QString username = Authmanager::getLoggedInUsername();
    UserDataFileManager::saveUserData(username, playlists, FaveriteTracks, tracksFromQueue);
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

void MusicRoom::showPlayListTracks(const QString& playlistName) {
    if (viewMode != ViewMode::PlayList)
        return;
    // MessageDisplayer::display(MessageType::Info, "Notice", "Show Playlist " + playlistName);
    for (auto& playlist : playlists)
    {
        if (playlist.name == playlistName)
            addTracksToListView(playlist.tracks);
    }
}

void MusicRoom::showFaveriteTracks() {
    qDebug() << "----Showing favorite tracks, count:----" << FaveriteTracks.size();

    if (viewMode != ViewMode::Faverite)
       return;
       addTracksToListView(FaveriteTracks);
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
                       /// update in file
    QString username = Authmanager::getLoggedInUsername();
    UserDataFileManager::saveUserData(username, playlists, FaveriteTracks, tracksFromQueue);

    return true;
}


bool MusicRoom::addTrackToFaverite(AudioTrack& trackToBeAdded)
{
    qDebug() << "----Loaded FaveriteTracks count:----" << FaveriteTracks.size();

    for (auto& track : FaveriteTracks)
        if (track.name == trackToBeAdded.name)
            return false;

    FaveriteTracks.append(trackToBeAdded);
        /// update in file
    QString username = Authmanager::getLoggedInUsername();
    UserDataFileManager::saveUserData(username, playlists, FaveriteTracks, tracksFromQueue);
    return true;
}

void MusicRoom::addMessageToChatbox(const QString& username, const QString& msg)
{
    QString safeMessage = msg.toHtmlEscaped();
    QString formatted;

    if (username == "Server")
        formatted = QString("<b> <span>%1</span>: <i>%2</i></b>").arg(username, safeMessage);
    else
        formatted = QString("<b>%1</b>: <span>%2</span>").arg(username, safeMessage);

    ui->TextBrowser_Messages->append(formatted);
    ui->TextBrowser_Messages->moveCursor(QTextCursor::End);
}

void MusicRoom::disableClientUI()
{
    ui->TreeWidget_Category->setEnabled(false);
    ui->ListView_AudioTracks->setEnabled(false);
    ui->PushButton_AddTrackToPlayList->setEnabled(false);
    ui->PushButton_AddTrackToQueue->setEnabled(false);
    ui->PushButton_AddTrackToFavorite->setEnabled(false);
    ui->GroupBox_CreatePlaylist->setEnabled(false);
    ui->Groupbox_Controls->setEnabled(false);
    ui->PushButton_Browse->setEnabled(true);
}

void MusicRoom::playMusicOnline(int index)
{
    if (server->clientsWhoSetTheirFolder.size() != server->clients.size())
    {
        Command msg(CommandType::Message, "Server", "Set your folders please.");
        server->broadcastMessage(msg);
        addMessageToChatbox("Server", "Set your folders please.");
        
        MessageDisplayer::display(MessageType::Critical, "Error", "Not everybody has set folders");
        return;
    }

    server->currentTrackName = tracksInListView[index]->getName();
    server->currentTrackPath = tracksInListView[index]->getPath();
    
    Command cmd(CommandType::WannaPlayThisTrack, "", server->currentTrackName);
    qDebug() << "asked for wannaplaythissong";
    server->broadcastMessage(cmd);
}

bool fileExistsInFolder(QString folderPath, QString fileName) 
{
    QDir dir(folderPath);
    return dir.exists(fileName);
}


void MusicRoom::on_Label_Timer_linkActivated(const QString &link)
{
    ui->Label_Timer->raise();

}

//void on_TreeWidget_Category_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)}{}

