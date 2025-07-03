#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <QMainWindow>
#include "../Network/ServerLogic/server.h"
#include "../user.h"
#include <QDateTime>
#include <memory>

namespace Ui {
class HostWindow;
}

class HostWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HostWindow(QWidget *parent = nullptr);
    ~HostWindow();

private slots:
    void on_PushButton_Back_clicked();
    void on_PushButton_CreateRoom_clicked();
    void on_PushButton_GoToMusicRoom_clicked();

    void on_serverStarted(int port);
    void on_serverStopped();
    void on_serverDeleted();
    void on_serverError(const QString& errorMessage);
    void on_clientConnected(const QString& username, int clientsCount);
    void on_clientDisconnection(const QString& username, int clientsCount);
    void on_dataReceived(const QString& username, const QByteArray& message);
    void on_clientConnectedToMainServer();
    void on_messageReceived(const QString& username, const QString& content);

    void on_LineEdit_RoomName_editingFinished();

private:
    Ui::HostWindow *ui;
    Server* server;
    bool hasSetupUIConnections;
    
    void connectServerSignalsToUISlots(const Server* server);
    void updateClientsList();
};

#endif // HOSTWINDOW_H
