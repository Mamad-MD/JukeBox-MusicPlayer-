#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QMainWindow>

namespace Ui {
class LoginWindow;
}

class Authmanager;  // فرض می‌کنیم داری استفاده می‌کنی

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(Authmanager *authManager, QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void goToSignup();
    void goToForgotPassword();

private slots:
    void handleLogin();

private:
    Ui::LoginWindow *ui;
    Authmanager *auth;
};

#endif // LOGINWINDOW_H
