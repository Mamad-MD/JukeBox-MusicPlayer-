#include "joinwindow.h"
#include "mainwindow.h"
#include "ui_joinwindow.h"

JoinWindow::JoinWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JoinWindow)
{
    ui->setupUi(this);
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

