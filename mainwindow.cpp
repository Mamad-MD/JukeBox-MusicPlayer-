#include "mainwindow.h"
#include "hostwindow.h"
#include "joinwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PushButton_Host_clicked()
{
    auto hostWindow = new HostWindow;
    hostWindow->show();
    this->close();
}


void MainWindow::on_PushButton_Join_clicked()
{
    auto joinWindow = new JoinWindow;
    joinWindow->show();
    this->close();
}

