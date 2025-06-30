#include "musicroom.h"
#include "ui_musicroom.h"
#include "message_displayer.h"

MusicRoom::MusicRoom(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MusicRoom), hasSetFolder(false), hasAQueue(false), model(nullptr), isPlaying(false)
{
    ui->setupUi(this);
    for (int i = 0; i < ui->TreeWidget_Category->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* topItem = ui->TreeWidget_Category->topLevelItem(i);
        categoryItems[i] = topItem;
        // iterateItemsInTree(topItem);
    }
    
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);


    connect(player, &QMediaPlayer::mediaStatusChanged, this, [](QMediaPlayer::MediaStatus status) {
        qDebug() << "Media status:" << status;
    });

    connect(player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
        qDebug() << "Error:" << error << errorString;
    });
    
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

void MusicRoom::on_TreeWidget_Category_itemActivated(QTreeWidgetItem *item, int column)
{
    if (item == categoryItems[0])
    {
        if (!hasSetFolder)
        {
            MessageDisplayer::display(MessageType::Info, "Notice", "You haven't selected a folder. First select one.");
            loadFolderSelection();
            return;
        }
        return;
    }

    if (item->parent() == categoryItems[1])
    {
        MessageDisplayer::display(MessageType::Info, "Notice", "it's a playlist");
        return;
    }

    if (item == categoryItems[2])
    {
        MessageDisplayer::display(MessageType::Info, "Notice", "Queue");
        return;
    }
}

void MusicRoom::loadFolderSelection()
{
    
}

void MusicRoom::on_PushButton_Browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->LineEdit_Path->setText(dir);
    showFolderTracks();
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
        delete model;

    model = new QStringListModel(this);

    model->setStringList(fileNames);
    ui->ListView_AudioTracks->setModel(model);
}

void MusicRoom::on_ListView_AudioTracks_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= musicPathsFromFolder.size())
        return;

    QString filePath = musicPathsFromFolder.at(index.row());
    play(filePath);
}

void MusicRoom::play(const QString& filePath)
{
    if (filePath == "")
    {
        player->play(); //resume
        isPlaying = true;
        on_musicPlayed();
        return;
    }

    // player->stop();
    player->setSource(QUrl::fromLocalFile(filePath));
    audioOutput->setVolume(0.5);
    player->play();
    isPlaying = true;
    currentlyPlayingPath = filePath;
    currentlyPlayingIndex = findIndexFromPath(filePath);
    qDebug() << "index: " << currentlyPlayingIndex;
    qDebug() << player->playbackState();
    on_musicPlayed();
}

void MusicRoom::pause()
{
    player->pause();
    isPlaying = false;
    on_musicPaused();
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

void MusicRoom::on_PushButton_PlayPause_clicked()
{
    if (isPlaying)
        pause();
    else
        play();
}

void MusicRoom::on_musicPlayed()
{
    ui->PushButton_PlayPause->setText("Pause");
}

void MusicRoom::on_musicPaused()
{
    ui->PushButton_PlayPause->setText("Play");
}


void MusicRoom::on_PushButton_Next_clicked()
{
    if (currentlyPlayingIndex == musicPathsFromFolder.size() - 1)
    {
        play(musicPathsFromFolder[0]);
        changeActiveTrackInListView(0);
    }
    else
    {
        play(musicPathsFromFolder[currentlyPlayingIndex + 1]);
        changeActiveTrackInListView(currentlyPlayingIndex);
    }
}


void MusicRoom::on_PushButton_Prev_clicked()
{
    if (currentlyPlayingIndex == 0)
    {
        play(musicPathsFromFolder[musicPathsFromFolder.size() - 1]);
        changeActiveTrackInListView(musicPathsFromFolder.size() - 1);
    }
    else
    {
        play(musicPathsFromFolder[currentlyPlayingIndex - 1]);
        changeActiveTrackInListView(currentlyPlayingIndex);
    }
}

