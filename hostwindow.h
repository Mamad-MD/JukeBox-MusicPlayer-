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

private:
    Ui::HostWindow *ui;
};

#endif // HOSTWINDOW_H
