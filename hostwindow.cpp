#include "hostwindow.h"
#include "mainwindow.h"
#include "ui_hostwindow.h"

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


void HostWindow::on_textEdit_textChanged()
{

}

