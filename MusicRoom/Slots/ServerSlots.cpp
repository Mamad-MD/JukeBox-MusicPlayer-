#include "../musicroom.h"
#include "ui_musicroom.h"
#include "../../MessageDisplayer/message_displayer.h"

void MusicRoom::on_clientsAllJoined()
{
    QStringList names;
    for (auto& client : server->clients)
        names << client.getUsername() << " ";
    addClientsToListView(names.join(" "));
}
