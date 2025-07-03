#include "../musicroom.h"
#include "ui_musicroom.h"
#include "../../MessageDisplayer/message_displayer.h"

void MusicRoom::clearClientsList()
{
    if (!modelForClientListView)
        return;
    modelForClientListView->setStringList(QStringList());
    ui->ListView_Users->setModel(modelForClientListView);
}

void MusicRoom::addClientsToListView(const QString& names)
{
    clearClientsList();
    QStringList usernames = names.split(" ", Qt::SkipEmptyParts);

    if (!modelForClientListView)
        modelForClientListView = new QStringListModel(this);

    modelForClientListView->setStringList(usernames);
    ui->ListView_Users->setModel(modelForClientListView);
}

void MusicRoom::on_clientNamesReceived(const QString& names)
{
    addClientsToListView(names);
}

void MusicRoom::on_newMessageReceived(const QString& username, const QString& msg)
{
    addMessageToChatbox(username, msg);
}


void MusicRoom::on_PushButton_Send_clicked()
{
    if (ui->TextEdit_Message->toPlainText().isEmpty())
        return;


    QString message = ui->TextEdit_Message->toPlainText();
    if (server)
    {
        Command msg(CommandType::Message, "Server", message);
        server->broadcastMessage(msg);
        addMessageToChatbox("Server", message);
    }
    else
    {
        Command msg(CommandType::Message, client->username, message);
        client->sendCommand(msg);
    }

    ui->TextEdit_Message->setText("");
}

void MusicRoom::sayWhetherIHaveThisTrack(QString& trackName)
{
    QDir dir(client->folderPath);

    if (dir.exists(trackName))
    {
        Command cmd(CommandType::ThisTrackStatusOnMyPC, "", "1");
        if (client->file)
            client->file->close();
        client->sendCommand(cmd);
    }
    else
    {
        Command cmd(CommandType::ThisTrackStatusOnMyPC, "", "0");
        client->sendCommand(cmd);
    }

}

void MusicRoom::on_playTheTrackOnline()
{
    QString fullPath = QDir(client->folderPath).filePath(client->currentTrackName);
    qDebug() << "path: " + fullPath;
    if (client->file)
        client->file->close();
    if (currentTrack)
    {
        if (currentTrack->name == client->currentTrackName)
        {
            musicPlayer->play();
            return;
        }
        else
        {
            delete currentTrack;
            currentTrack = new AudioTrack(client->currentTrackName, fullPath);
        }
    }
    else
        currentTrack = new AudioTrack(client->currentTrackName, fullPath);

    currentTrack = new AudioTrack(client->currentTrackName, fullPath);

    
    musicPlayer->setAudioTrack(currentTrack);
    musicPlayer->play();
}

void MusicRoom::on_pauseTheTrackOnline()
{
    musicPlayer->pause();
}
