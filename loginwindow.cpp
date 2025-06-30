#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QPalette>
#include <QBrush>
#include <QPixmap>
#include "authmanager.h"


LoginWindow::LoginWindow(Authmanager *authManager, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow),
    auth(authManager)
{
    ui->setupUi(this);

    // تنظیم سیگنال دکمه‌ها
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(ui->goToSignupButton, &QPushButton::clicked, this, &LoginWindow::goToSignup);
    connect(ui->forgotPasswordButton, &QPushButton::clicked, this, &LoginWindow::goToForgotPassword);

    // تنظیم پس زمینه تصویر (کد جایگزین styleSheet برای اطمینان)
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, QBrush(QPixmap(":/images/login.jpg").scaled(this->size(), Qt::KeepAspectRatioByExpanding)));
    this->setPalette(pal);
    this->setAutoFillBackground(true);

    // تنظیم پنل شفاف (loginPanel باید در ui داشته باشی)
    ui->loginPanel->setStyleSheet("background-color: rgba(255, 255, 255, 180); border-radius: 10px;");
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::handleLogin()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (auth->validateLogin(username, password)) {
        QMessageBox::information(this, "Login", "Login successful!");
    } else {
        QMessageBox::warning(this, "Login", "Invalid username or password.");
    }
}
