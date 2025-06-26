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

private slots:
    void updateRoomsList();
    void on_PushButton_Back_clicked();

private:
    Ui::JoinWindow *ui;
};

#endif // JOINWINDOW_H
