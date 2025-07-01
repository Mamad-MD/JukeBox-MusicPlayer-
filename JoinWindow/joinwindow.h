#ifndef JOINWINDOW_H
#define JOINWINDOW_H

#include <QMainWindow>
#include "client.h"

namespace Ui {
class JoinWindow;
}

class JoinWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit JoinWindow(QWidget *parent = nullptr);
    ~JoinWindow();

private slots:
    void on_PushButton_Back_clicked();
    void on_PushButton_LookForRooms_clicked();
    
    void updateRoomsList(const QString& roomName);
    // Network Slots:
    void on_connectedToServer();
    void on_ReceivedRoomName(const QString& roomName);
    void on_disconnection();
    void on_dataReceived(const QByteArray& data);
    void on_clientError(const QString& errorMessage);
    void on_clientObjectDeleted();

    void on_TableWidget_Rooms_cellClicked(int row, int column);

private:
    Ui::JoinWindow *ui;
    
    Client* client;
    bool hasSetupUIConnections;
    void connectClientSignalsToUISlots(const Client* client);
};

#endif // JOINWINDOW_H
