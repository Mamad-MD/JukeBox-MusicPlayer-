#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>

class VisualizerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VisualizerWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // VISUALIZERWIDGET_H
