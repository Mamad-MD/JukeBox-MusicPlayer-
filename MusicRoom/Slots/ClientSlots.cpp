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
