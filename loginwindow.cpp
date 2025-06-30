#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "authmanager.h"
#include <QMessageBox>
#include <QPixmap>


LoginWindow::LoginWindow(Authmanager *authManager, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LoginWindow),
    auth(authManager)
{
    ui->setupUi(this);

    QPixmap bg(":/images/login.jpg");
    ui->backgroundLabel->setPixmap(bg.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    this->setFixedSize(800, 600);
    ui->loginPanel->setStyleSheet("background-color: rgba(255, 255, 255, 180); border-radius: 10px;");

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(ui->goToSignupButton, &QPushButton::clicked, this, &LoginWindow::goToSignup);
    connect(ui->forgotPasswordButton, &QPushButton::clicked, this, &LoginWindow::goToForgotPassword);
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
        emit loginSucceeded(username);
    } else {
        QMessageBox::warning(this, "Login", "Invalid username or password.");
    }
}
