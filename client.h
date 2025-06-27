#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class Client : public QObject {
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    void sendMessage(const QString& message);

signals:
    void connectedToServer(); // Called when connected to the main server
    void receivedRoomName(QString roomName); // Called when room names are received
    void disconnection();

private slots:
    void connected();  // Called when connection is established

    void readData();   // Called when data is received
    void disconnected();

private:
    QString lastReceivedMessage;
    QTcpSocket* socket;
    bool hasReceivedRoomName;

};

#endif // CLIENT_H
