#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "authmanager.h"
#include "../MessageDisplayer/message_displayer.h"

LoginWindow::LoginWindow(Authmanager *authManager, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LoginWindow),
    auth(authManager)
{
    ui->setupUi(this);

    QPixmap bg(":/images/login.jpg");
    ui->backgroundLabel->setPixmap(bg.scaled(800, 450, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    this->setFixedSize(800, 450);
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
                Authmanager::UserData userData = auth->getUserData(username);
        MessageDisplayer::display(MessageType::Info, "Login successful!",   QString("hi %1 %2!\nWelcome to JukeBox :)")
                                                                              .arg(userData.firstname)
                                                                              .arg(userData.lastname));
        emit loginSucceeded(username);
        QFile file("loggedin_user.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << username.trimmed();
            file.close();
        }

    } else {
        MessageDisplayer::display(MessageType::Warning, "Login", "Invalid username or password.");
    }
}
