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




























