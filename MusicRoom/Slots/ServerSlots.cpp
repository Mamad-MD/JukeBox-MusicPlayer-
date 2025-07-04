#include "../musicroom.h"
#include "ui_musicroom.h"
#include "../../MessageDisplayer/message_displayer.h"

void MusicRoom::on_clientsAllJoined()
{
    QStringList names;
    for (auto& client : server->clients)
        names << client.getUsername() << " ";
    names << "Server";
    addClientsToListView(names.join(" "));
}

void MusicRoom::on_clientDisconnected(const QString& username, int clientsCount)
{
    QString names = server->getUsernamesAsQString();
    names.append("Server");
    Command command(CommandType::ClientNames_Sending, "", names);
    server->broadcastMessage(command);
    
    addClientsToListView(names);
    
    addMessageToChatbox("Server", username + " disconnected.");
}

void MusicRoom::on_messageReceived(const QString& username, const QString& msg)
{
    addMessageToChatbox(username, msg);
}

void MusicRoom::on_allSetTheirFolders()
{
    MessageDisplayer::display(MessageType::Info, "Error", "All set their folders");
}

void MusicRoom::on_notHaveTheTrack(QTcpSocket* receiver)
{
    QFile *file = new QFile(server->currentTrackPath);
    if (file->open(QIODevice::ReadOnly)) {
        QByteArray buffer;
        while (!file->atEnd()) {
            buffer = file->read(4096); // 4 KB chunks
            receiver->write(buffer);
            receiver->waitForBytesWritten();
        }
    }

    server->clientsWhoHaveTheTrack.append(server->findClientBySocket(receiver));
    if (server->clientsWhoHaveTheTrack.size() == server->clients.size())
        emit server->allHaveTheTrack();
}

void MusicRoom::on_allHaveTheTrack()
{
    MessageDisplayer::display(MessageType::Info, "Notice", "Everybody has the track");
    Command cmd(CommandType::Play_Request, "", "");
    server->broadcastMessage(cmd);
    int index = 0;
    for (int i = 0; i < tracksInListView.size(); i++)
        if (tracksInListView[i]->getName() == server->currentTrackName)
        {
            index = i;
            break;
        }
        
    musicPlayer->setAudioTrack(PlayList::findTrackInListByName(tracksInListView, server->currentTrackName));
    currentlyPlayingIndex = index;
    musicPlayer->play();
}

















