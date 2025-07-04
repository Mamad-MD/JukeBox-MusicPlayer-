#include "visualizerwidget.h"
#include <QPainter>
#include <QTimer>
#include <cstdlib>

VisualizerWidget::VisualizerWidget(QWidget *parent) : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&VisualizerWidget::update));
    timer->start(100);
}

void VisualizerWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(20, 20, 20));

    int barCount = 20;
    int spacing = 5;
    int barWidth = (width() - spacing * (barCount - 1)) / barCount;
    int maxHeight = height();

    for (int i = 0; i < barCount; ++i) {
        int heightValue = (std::rand() % (maxHeight - 20)) + 20;
        QRect barRect(i * (barWidth + spacing), maxHeight - heightValue, barWidth, heightValue);
        QColor color(50 + std::rand() % 200, 200, 100);
        painter.fillRect(barRect, color);
    }
}
