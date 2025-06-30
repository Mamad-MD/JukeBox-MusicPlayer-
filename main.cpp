#include "mainwindow.h"
#include <QApplication>
#include "loginwindow.h"
#include "authmanager.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Authmanager auth;
    LoginWindow loginWin(&auth);
    MainWindow mainWin;
    QObject::connect(&loginWin, &LoginWindow::loginSucceeded, [&](const QString &) {
        mainWin.show();
        loginWin.close();
    });

    loginWin.show();

    return app.exec();
}
