#include "joinwindow.h"
#include "mainwindow.h"
#include "ui_joinwindow.h"
#include "client.h"
#include <QTimer>

JoinWindow::JoinWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JoinWindow)
{

    ui->setupUi(this);

    updateRoomsList();

    QTimer* timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &updateRoomsList);
    timer->start(1000);
}

JoinWindow::~JoinWindow()
{
    delete ui;
}

void JoinWindow::on_PushButton_Back_clicked()
{
    auto mainWindow = new MainWindow;
    mainWindow->show();
    this->close();
}

void JoinWindow::updateRoomsList()
{
    Client client;

}


