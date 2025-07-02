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
    ui->Widget_MusicDisplay->setCurrentIndex(0); // اکولایزر
    //ui->Label_AlbumCover->hide();        // کاور مخفی
    //visualizer->show();
    ui->PushButton_SwitchView->setText("Visualizer");
    isShowingCover = true;
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
        ui->PushButton_PlayPause->setText("Play");
        if (shuffleOn)
        {
            switch (repeatType)
            {
            case RepeatType::No_Repeat:
                playRandomIndex();
                break;

            case RepeatType::Repeat_Track:
                // Here shuffle has no effect.
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
        ui->PushButton_Shuffle->setText("Shuffle: Off");
    }
    else
    {
        shuffleOn = true;
        ui->PushButton_Shuffle->setText("Shuffle: On");

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
            ui->Label_AlbumCover->show();  // اطمینان از نمایش کاور
            qDebug() << "Cover image set!";
        } else {
            ui->Label_AlbumCover->clear();
            qDebug() << "Image variant is null.";
        }
    } else {
        ui->Label_AlbumCover->clear();
    }

//     if (cover.isValid()) {
//         QImage image = qvariant_cast<QImage>(cover);
//         if (!image.isNull()) {
//             ui->Label_AlbumCover->setPixmap(QPixmap::fromImage(image));
//             qDebug() << "Cover image set!";
//         } else {
//             qDebug() << "Image variant is null.";
//             ui->Label_AlbumCover->clear();
//         }
//     } else {
//         ui->Label_AlbumCover->clear();
//      }
}

void MusicRoom::on_PushButton_SwitchView_clicked()
{
    qDebug() << "SwitchView clicked. isShowingCover:" << isShowingCover;

    if (isShowingCover) {
        ui->Widget_MusicDisplay->setCurrentIndex(1); // اکولایزر
                                 ui->Label_AlbumCover->hide();
                                 visualizer->show();
        ui->PushButton_SwitchView->setText("Album Cover");
        //isShowingCover = false;
        qDebug() << "Switched to Visualizer";

    } else {
        ui->Widget_MusicDisplay->setCurrentIndex(0); // کاور
                                        visualizer->hide();
                                       ui->Label_AlbumCover->show();
        ui->PushButton_SwitchView->setText("Visualizer");
       // isShowingCover = true;
        qDebug() << "Switched to Album Cover";

    }
    isShowingCover = !isShowingCover;
    qDebug() << "New isShowingCover:" << isShowingCover;

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
