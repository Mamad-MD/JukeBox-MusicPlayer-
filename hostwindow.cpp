#include "hostwindow.h"
#include "mainwindow.h"
#include "ui_hostwindow.h"
#include "message.h"
#include "server.h"

HostWindow::HostWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HostWindow)
{
    ui->setupUi(this);
}

HostWindow::~HostWindow()
{
    delete ui;
}

void HostWindow::on_PushButton_Back_clicked()
{
    auto mainWindow = new MainWindow;
    mainWindow->show();
    this->close();
}

void HostWindow::on_PushButton_CreateRoom_clicked()
{
    if (ui->LineEdit_RoomName->text().isEmpty())
    {
        Message::display(MessageType::Critical, "Error", "Room name cannot be empty!");
        return;
    }

    Server server;
}
