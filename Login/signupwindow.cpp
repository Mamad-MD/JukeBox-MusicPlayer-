#include "signupwindow.h"
#include "ui_signupwindow.h"
#include "../MessageDisplayer/message_displayer.h"

SignUpWindow::SignUpWindow(Authmanager* authManager, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::SignUpWindow),
    auth(authManager)
{
    ui->setupUi(this);
}

SignUpWindow::~SignUpWindow()
{
    delete ui;
}

void SignUpWindow::on_registerButton_clicked()
{
    QString firstname = ui->firstNameLineEdit->text().trimmed();
    QString lastname = ui->lastNameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();

    if (firstname.isEmpty() || lastname.isEmpty() || username.isEmpty()
        || password.isEmpty() || confirmPassword.isEmpty() || email.isEmpty())
    {
        MessageDisplayer::display(MessageType::Warning, "Registration", "Please fill in all the fields.");
        return;
    }

    if (password != confirmPassword) {
        MessageDisplayer::display(MessageType::Warning, "Registration", "Password and confirmation do not match.");
        return;
    }


    bool success = auth->registerUser(firstname, lastname, username, password, email);

    if (success) {
        MessageDisplayer::display(MessageType::Info, "Registration", "Registration successful.");

        emit registrationSucceeded();

        this->hide();

    } else {
        MessageDisplayer::display(MessageType::Critical, "Registration", "Username is already taken.");
    }
}
