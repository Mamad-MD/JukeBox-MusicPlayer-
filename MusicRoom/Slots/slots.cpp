#include "../musicroom.h"
#include "ui_musicroom.h"
#include "../../MessageDisplayer/message_displayer.h"
#include "ui_musicroom.h"
#include "Login/authmanager.h"
#include "Login/userdatafilemanager.h".h"


// Slots

void MusicRoom::on_PushButton_PlayPause_clicked()
{
    if (server)
    {
        if (server->clientsWhoHaveTheTrack.size() != server->clients.size())
            return;
        if (musicPlayer->isPlaying)
        {
            Command cmd(CommandType::Pause_Request, "", "");
            server->broadcastMessage(cmd);
            musicPlayer->pause();
        }
        else
        {
            Command cmd(CommandType::Play_Request, "", "");
            server->broadcastMessage(cmd);
            musicPlayer->play();
        }
        return;
    }

    if (musicPlayer->isPlaying)
        musicPlayer->pause();
    else
        musicPlayer->play();
}

void MusicRoom::on_musicPlayed()
{
    ui->Widget_MusicDisplay->setCurrentIndex(0);
    ui->PushButton_SwitchView->setText("Visualizer");
    isShowingCover = true;
    isPlaying = true;
    ui->PushButton_PlayPause->setIconSize(QSize(40, 40));
    ui->PushButton_PlayPause->setIcon(pauseIcon);
}

void MusicRoom::on_musicPaused()
{
   isPlaying = false;
   ui->PushButton_PlayPause->setIconSize(QSize(40, 40));
   ui->PushButton_PlayPause->setIcon(playIcon);
}

void MusicRoom::on_positionChanged(qint64 position)
{
    ui->Slider_MusicSlider->setValue(static_cast<int>(position));
    QString currentTimeStr = formatTime(position);
    QString totalTimeStr = formatTime(currentlyPlayingDuration);
    ui->Label_Timer->setText(currentTimeStr + " / " + totalTimeStr);
    if (position == currentlyPlayingDuration)
    {
        ui->PushButton_PlayPause->setText("Play");
        if (server || client)
            return;
        if (shuffleOn)
        {
            switch (repeatType)
            {
            case RepeatType::No_Repeat:
                playRandomIndex();
                break;

            case RepeatType::Repeat_Track:
                playThisIndex(currentlyPlayingIndex);
                break;

            case RepeatType::Repeat_List:
                playRandomIndex();
                break;
            }
        }
        else
        {
            switch (repeatType)
            {
            case RepeatType::No_Repeat:
                if (currentlyPlayingIndex != tracksInListView.size() - 1)
                    playNext();
                break;

            case RepeatType::Repeat_Track:
                playThisIndex(currentlyPlayingIndex);
                break;

            case RepeatType::Repeat_List:
                playNext();
                break;
            }
        }
    }
}

void MusicRoom::on_durationChanged(qint64 duration)
{
    currentlyPlayingDuration = duration;
    ui->Slider_MusicSlider->setMaximum(static_cast<int>(duration));
}

void MusicRoom::on_Slider_MusicSlider_sliderReleased()
{
    musicPlayer->player->setPosition(ui->Slider_MusicSlider->value());
}

void MusicRoom::on_PushButton_Next_clicked()
{

    playNext();
}


void MusicRoom::on_PushButton_Prev_clicked()
{

    playPrev();
}

void MusicRoom::on_PushButton_Shuffle_clicked()
{
    if (shuffleOn)
    {
        shuffleOn = false;
        ui->PushButton_Shuffle->setText(" Off");
    }
    else
    {
        shuffleOn = true;
        ui->PushButton_Shuffle->setText(" On");

    }
}

void MusicRoom::on_metaDataChanged()
{
    qDebug() << "metaDataChanged() called!";
    const QMediaMetaData meta = musicPlayer->player->metaData();
    const QList<QMediaMetaData::Key> metaKeys = meta.keys();
    qDebug() << "All meta keys:" << metaKeys;

    QVariant cover;

    if (metaKeys.contains(QMediaMetaData::CoverArtImage)) {
        cover = meta.value(QMediaMetaData::CoverArtImage);
        qDebug() << "CoverArtImage found!";
    } else if (metaKeys.contains(QMediaMetaData::ThumbnailImage)) {
        cover = meta.value(QMediaMetaData::ThumbnailImage);
        qDebug() << "ThumbnailImage used as cover!";
    } else {
        qDebug() << "No image metadata found.";
    }
    if (cover.isValid()) {
        QImage image = qvariant_cast<QImage>(cover);
        if (!image.isNull()) {
            ui->Label_AlbumCover->setPixmap(QPixmap::fromImage(image).scaled(
                ui->Label_AlbumCover->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
            ui->Label_AlbumCover->show();
            qDebug() << "Cover image set!";
        } else {
            ui->Label_AlbumCover->clear();
            qDebug() << "Image variant is null.";
        }
    } else {
        ui->Label_AlbumCover->clear();
    }

}

void MusicRoom::on_PushButton_SwitchView_clicked()
{
    qDebug() << "SwitchView clicked. isShowingCover:" << isShowingCover;

    if (isShowingCover) {
        ui->Widget_MusicDisplay->setCurrentIndex(1);
                                 ui->Label_AlbumCover->hide();
                                 visualizer->show();
        ui->PushButton_SwitchView->setText("Album Cover");
        qDebug() << "Switched to Visualizer";

    } else {
        ui->Widget_MusicDisplay->setCurrentIndex(0);
                                        visualizer->hide();
                                       ui->Label_AlbumCover->show();
        ui->PushButton_SwitchView->setText("Visualizer");
        qDebug() << "Switched to Album Cover";

    }
    isShowingCover = !isShowingCover;
    qDebug() << "New isShowingCover:" << isShowingCover;

}

void MusicRoom::on_ListView_AudioTracks_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= tracksFromFolder.size())
        return;

    if (server)
    {
        playMusicOnline(index.row());
        return;
    }

    musicPlayer->setAudioTrack(tracksInListView[index.row()]);
    currentlyPlayingIndex = index.row();
    musicPlayer->play();
}

void MusicRoom::on_PushButton_Browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        ui->LineEdit_Path->setText(dir);
        hasSetFolder = true;
        if (client)
        {
            client->folderPath = dir;
            Command command(CommandType::SetMyFolder, "", dir);
            client->sendCommand(command);
        }
    }

    if (!client)
        showFolderTracks();
}

void MusicRoom::on_TreeWidget_Category_itemActivated(QTreeWidgetItem *item, int column)
{
    if (item == categoryItems[0])
    {
        if (!hasSetFolder)
            MessageDisplayer::display(MessageType::Info, "Notice", "You haven't selected a folder. First select one.");
        
        viewMode = ViewMode::Folder;
        activePlaylist = "";
        ui->PushButton_Browse->setEnabled(true);
        musicPlayer->clearAudioTrack();
        showFolderTracks();
        return;
    }
    else
        ui->PushButton_Browse->setEnabled(false);

    if (item->parent() == categoryItems[1])
    {
        viewMode = ViewMode::PlayList;
        activePlaylist = item->text(0);
        clearTracksListView();
        musicPlayer->clearAudioTrack();
        showPlayListTracks(activePlaylist);
        return;
    }

    if (item == categoryItems[2])
    {
        currentlyPlayingIndex = -1;
        currentlyPlayingDuration = 0;
        viewMode = ViewMode::Queue;
        activePlaylist = "";
        musicPlayer->clearAudioTrack();
        clearTracksListView();
        showQueueTracks();
        return;
    }
    if(item == categoryItems[3])
    {
        currentlyPlayingIndex = -1;
        currentlyPlayingDuration = 0;
        viewMode = ViewMode::Faverite;
        activePlaylist = "";
        musicPlayer->clearAudioTrack();
        clearTracksListView();
        showFaveriteTracks();
        return;
    }
}


void MusicRoom::on_PushButton_Repeat_clicked()
{
    int currentType = static_cast<int>(repeatType);
    if (currentType == 2)
        currentType = 0;
    else
        currentType++;
    repeatType = static_cast<RepeatType>(currentType);
    switch (repeatType)
    {
    case RepeatType::No_Repeat:
    {
        ui->PushButton_Repeat->setText("Repeat Mode: No-Repeat");
        break;
    }

    case RepeatType::Repeat_Track:
    {
        ui->PushButton_Repeat->setText("Repeat Mode: Repeat-Track");
        break;
    }

    case RepeatType::Repeat_List:
    {
        ui->PushButton_Repeat->setText("Repeat Mode: Repeat-List");
        break;
    }
    }
}


void MusicRoom::on_Slider_Volume_valueChanged(int value)
{
    musicPlayer->setVolume((float)value / 100);
}

void MusicRoom::on_PushButton_CreatePlayList_clicked()
{
    if (ui->LineEdit_PlayListName->text().isEmpty())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "PlayList name cannot be empty!");
        return;
    }
    
    QString playlistName = ui->LineEdit_PlayListName->text();
    ui->LineEdit_PlayListName->setText("");
    playlists.append(PlayList(playlistName));
    reloadPlaylistsInListView();
    reloadPlaylistsInTree();
}


void MusicRoom::on_PushButton_AddTrackToPlayList_clicked()
{
    if (tracksInListView.empty() || playlists.empty())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Either track list or playlists list is empty!");
        return;
    }

    QModelIndex selectedTrackIndex = ui->ListView_AudioTracks->currentIndex();
    QModelIndex selectedPlaylistIndex = ui->ListView_PlayLists->currentIndex();

    if (!selectedTrackIndex.isValid() || !selectedPlaylistIndex.isValid())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Invalid selection!");
        return;
    }

    QString trackName = selectedTrackIndex.data(Qt::DisplayRole).toString();
    QString playlistName = selectedPlaylistIndex.data(Qt::DisplayRole).toString();

    PlayList& playlist = *findPlaylistByName(playlistName);
    AudioTrack& track = *PlayList::findTrackInListByName(tracksInListView, trackName);

    if (!playlist.addTrack(track))
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Selected track is already in the playlist!");
        return;
    }

    QString username = Authmanager::getLoggedInUsername();
    UserDataFileManager::saveUserData(username, playlists, FaveriteTracks, tracksFromQueue);
    if (activePlaylist == playlistName && viewMode == ViewMode::PlayList)
        showPlayListTracks(activePlaylist);
}


void MusicRoom::on_PushButton_AddTrackToQueue_clicked()
{
    if (tracksInListView.empty())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Track list is empty!");
        return;
    }

    QModelIndex selectedTrackIndex = ui->ListView_AudioTracks->currentIndex();
    QString trackName = selectedTrackIndex.data(Qt::DisplayRole).toString();
    AudioTrack& track = *PlayList::findTrackInListByName(tracksInListView, trackName);

    if (!addTrackToQueue(track))
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Selected track is already in the queue!");
        return;
    }

    if (viewMode == ViewMode::Queue)
        showQueueTracks();
}

void MusicRoom::on_PushButton_AddTrackToFavorite_clicked()
{
       qDebug() << "Add to Favorite clicked!";
    if (tracksInListView.empty())
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Track list is empty!");
        return;
    }

    QModelIndex selectedTrackIndex = ui->ListView_AudioTracks->currentIndex();
    QString trackName = selectedTrackIndex.data(Qt::DisplayRole).toString();
    AudioTrack& track = *PlayList::findTrackInListByName(tracksInListView, trackName);


    qDebug() << "-----Trying to add track to favorite:--------" << track.name;


    if (!addTrackToFaverite(track))
    {
        MessageDisplayer::display(MessageType::Critical, "Error", "Selected track is already in the Faverite!");
        return;
    }

    qDebug() << "----Track added to favorite successfully.-------";


    if (viewMode == ViewMode::Faverite)
        showFaveriteTracks();
}
