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

    // لاگین موفق => نمایش پنجره اصلی، مخفی کردن لاگین
    QObject::connect(&loginWin, &LoginWindow::loginSucceeded, [&](const QString &) {
        mainWin.show();
        loginWin.hide();
    });

    // بازگشت به لاگین پس از بستن پنجره اصلی (دلخواه)
    QObject::connect(&mainWin, &MainWindow::destroyed, [&]() {
        loginWin.show();
    });

    // باز کردن صفحه ثبت نام
    QObject::connect(&loginWin, &LoginWindow::goToSignup, [&]() {
        signupWin.show();
        loginWin.hide();
    });

    // بازگشت از ثبت نام به لاگین بعد از موفقیت ثبت نام
    QObject::connect(&signupWin, &SignUpWindow::registrationSucceeded, [&]() {
        signupWin.hide();
        loginWin.show();
    });

    // باز کردن صفحه فراموشی رمز
    QObject::connect(&loginWin, &LoginWindow::goToForgotPassword, [&]() {
        forgotPassWin.show();
        loginWin.hide();
    });

    // بعد از بستن فراموشی رمز، بازگشت به صفحه لاگین
    QObject::connect(&forgotPassWin, &ForgotPasswordWindow::destroyed, [&]() {
        loginWin.show();
    });

    loginWin.show();

    return app.exec();
}
