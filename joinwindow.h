#ifndef JOINWINDOW_H
#define JOINWINDOW_H

#include <QMainWindow>

namespace Ui {
class JoinWindow;
}

class JoinWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit JoinWindow(QWidget *parent = nullptr);
    ~JoinWindow();

private:
    Ui::JoinWindow *ui;
};

#endif // JOINWINDOW_H
