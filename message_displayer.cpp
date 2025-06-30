#include "message_displayer.h"
#include <QApplication>
#include <QMessageBox>

void MessageDisplayer::display(const MessageType type, QString title, QString description)
{
    // The parent of the QMessage will be the active window which is the main window now
    QWidget* activeWindow = QApplication::activeWindow();

    switch (type)
    {
    case MessageType::Info :
        QMessageBox::information(activeWindow, title, description);
        break;
    case MessageType::Warning :
        QMessageBox::warning(activeWindow, title, description);
        break;
    case MessageType::Critical :
        QMessageBox::critical(activeWindow, title, description);
        break;
    }
}
