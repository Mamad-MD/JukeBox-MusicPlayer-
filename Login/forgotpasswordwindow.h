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

private slots:
    void handleRecover();

private:
    Ui::ForgotPasswordWindow *ui;
    Authmanager* auth;
};

#endif // FORGOTPASSWORDWINDOW_H
