#include <QApplication>
#include "authmanager.h"
#include "loginwindow.h"
#include "signupwindow.h"
#include "forgotpasswordwindow.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Authmanager auth;

    LoginWindow loginWin(&auth);
    SignUpWindow signupWin(&auth);
    ForgotPasswordWindow forgotPassWin(&auth);
    MainWindow mainWin;

    QObject::connect(&loginWin, &LoginWindow::loginSucceeded, [&](const QString &) {
        mainWin.show();
        loginWin.hide();
    });

    QObject::connect(&mainWin, &MainWindow::destroyed, [&]() {
        loginWin.show();
    });

    QObject::connect(&loginWin, &LoginWindow::goToSignup, [&]() {
        signupWin.show();
        loginWin.hide();
    });

    QObject::connect(&signupWin, &SignUpWindow::registrationSucceeded, [&]() {
        signupWin.hide();
        loginWin.show();
    });

    QObject::connect(&loginWin, &LoginWindow::goToForgotPassword, [&]() {
        forgotPassWin.show();
        loginWin.hide();
    });

    QObject::connect(&forgotPassWin, &ForgotPasswordWindow::destroyed, [&]() {
        loginWin.show();
    });

    loginWin.show();

    return app.exec();
}
