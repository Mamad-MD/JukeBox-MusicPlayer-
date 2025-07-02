#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QMainWindow>

namespace Ui {
class LoginWindow;
}

class Authmanager;

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(Authmanager *authManager, QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void goToSignup();
    void loginSucceeded(const QString &username);
    void goToForgotPassword();


private slots:
    void handleLogin();

private:
    Ui::LoginWindow *ui;
    Authmanager *auth;
};

#endif // LOGINWINDOW_H
