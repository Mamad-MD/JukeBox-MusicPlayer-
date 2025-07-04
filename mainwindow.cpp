#include "mainwindow.h"
#include "HostWindow/hostwindow.h"
#include "JoinWindow/joinwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        this->setFixedSize(800, 600);
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

