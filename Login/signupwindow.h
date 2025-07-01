#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QMainWindow>
#include "authmanager.h"

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

private slots:
    void on_registerButton_clicked();

private:
    Ui::SignUpWindow *ui;
    Authmanager* auth;
};

#endif // SIGNUPWINDOW_H
