#ifndef FORGOTPASSWORDWINDOW_H
#define FORGOTPASSWORDWINDOW_H

#include <QWidget>

namespace Ui {
class forgotpasswordwindow;
}

class forgotpasswordwindow : public QWidget
{
    Q_OBJECT

public:
    explicit forgotpasswordwindow(QWidget *parent = nullptr);
    ~forgotpasswordwindow();

private:
    Ui::forgotpasswordwindow *ui;
};

#endif // FORGOTPASSWORDWINDOW_H
