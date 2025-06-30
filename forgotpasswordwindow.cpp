#include "forgotpasswordwindow.h"
#include "ui_forgotpasswordwindow.h"

forgotpasswordwindow::forgotpasswordwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::forgotpasswordwindow)
{
    ui->setupUi(this);
}

forgotpasswordwindow::~forgotpasswordwindow()
{
    delete ui;
}
