#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject {
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    void sendMessage(const QString& message);

private slots:
    void connected();  // Called when connection is established
    void readData();   // Called when data is received

private:
    QString lastReceivedMessage;
    QTcpSocket* socket;

};

#endif // CLIENT_H
