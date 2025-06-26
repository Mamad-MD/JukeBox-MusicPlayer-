#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <QMainWindow>

namespace Ui {
class HostWindow;
}

class HostWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HostWindow(QWidget *parent = nullptr);
    ~HostWindow();

private slots:
    void on_PushButton_Back_clicked();

    void on_PushButton_CreateRoom_clicked();

private:
    Ui::HostWindow *ui;
};

#endif // HOSTWINDOW_H
