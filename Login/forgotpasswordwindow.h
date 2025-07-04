#ifndef FORGOTPASSWORDWINDOW_H
#define FORGOTPASSWORDWINDOW_H

#include <QMainWindow>
#include "authmanager.h"

namespace Ui {
class ForgotPasswordWindow;
}

class ForgotPasswordWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ForgotPasswordWindow(Authmanager* authManager, QWidget *parent = nullptr);
    ~ForgotPasswordWindow();
signals:
    void backToLoginRequested();
private slots:
    void handleRecover();
    void on_backToLoginButton_clicked();
private:
    Ui::ForgotPasswordWindow *ui;
    Authmanager* auth;
};

#endif // FORGOTPASSWORDWINDOW_H
