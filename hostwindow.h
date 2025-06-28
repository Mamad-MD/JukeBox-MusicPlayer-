#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <QMainWindow>
#include "server.h"
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

    // Slots for Server
    void on_serverStarted(int port);
    void on_serverStopped();
    void on_serverDeleted();
    void on_serverError(const QString& errorMessage);
    void on_clientConnected(const QString& username, int clientsCount);
    void on_clientDisconnection(const QString& username, int clientsCount);
    void on_dataReceived(const QString& username, const QByteArray& message);


private:
    Ui::HostWindow *ui;
    Server* server; // Smart Pointer
    bool hasSetupUIConnections;
    // Interesting fact is that you can't copy a unique_ptr in C++. so
    // when you wanna pass it to a function you must pass its reference
    void connectServerSignalsToUISlots(const Server* server);
};

#endif // HOSTWINDOW_H
