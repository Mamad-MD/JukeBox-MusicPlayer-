#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QMainWindow>
#include "authmanager.h"
#include "Login/loginwindow.h"

namespace Ui {
class SignUpWindow;
}

class SignUpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignUpWindow(Authmanager* authManager, QWidget *parent = nullptr);
    ~SignUpWindow();

signals:
    void registrationSucceeded();
    void backToLoginRequested();

private slots:
    void on_registerButton_clicked();
    void on_goToLoginButton_clicked();

private:
    Ui::SignUpWindow *ui;
    Authmanager* auth;
};

#endif // SIGNUPWINDOW_H
