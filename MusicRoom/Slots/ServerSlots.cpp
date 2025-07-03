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

    
    Command msg(CommandType::Message, "Server", username + " disconnected.");
    server->broadcastMessage(msg);

    addMessageToChatbox("Server", username + " disconnected.");
}

void MusicRoom::on_messageReceived(const QString& username, const QString& msg)
{
    addMessageToChatbox(username, msg);
}
