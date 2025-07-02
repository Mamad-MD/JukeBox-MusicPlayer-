#include "forgotpasswordwindow.h"
#include "ui_forgotpasswordwindow.h"
#include "../MessageDisplayer/message_displayer.h"
#include <QPushButton>

ForgotPasswordWindow::ForgotPasswordWindow(Authmanager* authManager, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ForgotPasswordWindow),
    auth(authManager)
{
    ui->setupUi(this);

    connect(ui->recoverButton, &QPushButton::clicked, this, &ForgotPasswordWindow::handleRecover);
}

ForgotPasswordWindow::~ForgotPasswordWindow()
{
    delete ui;
}

void ForgotPasswordWindow::handleRecover()
{
    QString email = ui->emailLineEdit->text().trimmed();

    if(email.isEmpty()) {
        MessageDisplayer::display(MessageType::Warning, "Recover Password", "Please enter your email.");
        return;
    }

    QString result = auth->recoverPassword(email);
    if(result.startsWith("[TEST MODE]")) {
        MessageDisplayer::display(MessageType::Info, "Recover Password", "Recovery info sent:\n" + result);
    }
    else {
        MessageDisplayer::display(MessageType::Warning, "Recover Password", "Email not found.");
    }
}
