#include "../musicroom.h"
#include "ui_musicroom.h"
#include "../../MessageDisplayer/message_displayer.h"
// Slots

void MusicRoom::on_PushButton_PlayPause_clicked()
{
    if (musicPlayer->isPlaying)
        musicPlayer->pause();
    else
        musicPlayer->play();
}

void MusicRoom::on_musicPlayed()
{
    ui->PushButton_PlayPause->setText("Pause");
}

void MusicRoom::on_musicPaused()
{
    ui->PushButton_PlayPause->setText("Play");
}

void MusicRoom::on_positionChanged(qint64 position)
{
    ui->Slider_MusicSlider->setValue(static_cast<int>(position));
    QString currentTimeStr = formatTime(position);
    QString totalTimeStr = formatTime(currentlyPlayingDuration);
    ui->Label_Timer->setText(currentTimeStr + " / " + totalTimeStr);
    if (position == currentlyPlayingDuration)
    {
        if (currentlyPlayingIndex == tracksFromFolder.size() - 1)
        {
            // musicPlayer->setAudioTrack();
            // musicPlayer->play(musicPathsFromFolder[0]);
            changeActiveTrackInListView(0);
        }
        else
        {
            // musicPlayer->setAudioTrack();
            // musicPlayer->play(musicPathsFromFolder[currentlyPlayingIndex + 1]);
            changeActiveTrackInListView(currentlyPlayingIndex);
        }
    }
}

void MusicRoom::on_durationChanged(qint64 duration)
{
    currentlyPlayingDuration = duration;
    ui->Slider_MusicSlider->setMaximum(static_cast<int>(duration));
}

void MusicRoom::on_sliderReleased()
{
    musicPlayer->player->setPosition(ui->Slider_MusicSlider->value());
}

void MusicRoom::on_PushButton_Next_clicked()
{
    if (currentlyPlayingIndex == tracksInListView.size() - 1)
    {
        currentlyPlayingIndex = 0;

        musicPlayer->setAudioTrack(tracksInListView[currentlyPlayingIndex]);
        musicPlayer->play();
        changeActiveTrackInListView(0);
    }
    else
    {
        currentlyPlayingIndex++;

        musicPlayer->setAudioTrack(tracksInListView[currentlyPlayingIndex]);
        musicPlayer->play();
        changeActiveTrackInListView(currentlyPlayingIndex);
    }
}


void MusicRoom::on_PushButton_Prev_clicked()
{
    if (currentlyPlayingIndex == 0)
    {
        currentlyPlayingIndex = tracksInListView.size() - 1;

        musicPlayer->setAudioTrack(tracksInListView[currentlyPlayingIndex]);
        musicPlayer->play();
        changeActiveTrackInListView(currentlyPlayingIndex);
    }
    else
    {
        currentlyPlayingIndex--;

        musicPlayer->setAudioTrack(tracksInListView[currentlyPlayingIndex]);
        musicPlayer->play();
        changeActiveTrackInListView(currentlyPlayingIndex);
    }
}

void MusicRoom::on_metaDataChanged()
{
    qDebug() << "metaDataChanged() called!";
    const QMediaMetaData meta = musicPlayer->player->metaData();
    const QList<QMediaMetaData::Key> metaKeys = meta.keys();
    qDebug() << "All meta keys:" << metaKeys;

    QVariant cover;

    // اول سعی می‌کنیم از CoverArtImage بگیریم، اگر نبود ThumbnailImage
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
            ui->Label_AlbumCover->setPixmap(QPixmap::fromImage(image));
            qDebug() << "Cover image set!";
        } else {
            qDebug() << "Image variant is null.";
        }
    } else {
        ui->Label_AlbumCover->clear();
    }
}

void MusicRoom::on_ListView_AudioTracks_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= tracksFromFolder.size())
        return;

    musicPlayer->setAudioTrack(&tracksFromFolder[index.row()]);
    currentlyPlayingIndex = index.row();
    musicPlayer->play();
}

void MusicRoom::on_PushButton_Browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->LineEdit_Path->setText(dir);
    showFolderTracks();
    hasSetFolder = true;
}

void MusicRoom::on_TreeWidget_Category_itemActivated(QTreeWidgetItem *item, int column)
{
    if (item == categoryItems[0])
    {
        if (!hasSetFolder)
            MessageDisplayer::display(MessageType::Info, "Notice", "You haven't selected a folder. First select one.");
        ui->PushButton_Browse->setEnabled(true);
        showFolderTracks();
        return;
    }
    else
        ui->PushButton_Browse->setEnabled(false);

    if (item->parent() == categoryItems[1])
    {
        MessageDisplayer::display(MessageType::Info, "Notice", "it's a playlist");
        clearTracksListView();
        showPlayListTracks(item->text(0));
        return;
    }

    if (item == categoryItems[2])
    {
        MessageDisplayer::display(MessageType::Info, "Notice", "Queue");
        clearTracksListView();
        showQueueTracks();
        return;
    }
}
