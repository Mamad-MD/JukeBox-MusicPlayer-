#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class client : public QObject {
    Q_OBJECT

public:
    client(QObject* parent = nullptr);
    void sendMessage(const QString& message);

private slots:
    void connected();  // Called when connection is established
    void readData();   // Called when data is received

private:
    QTcpSocket* socket;

};

#endif // CLIENT_H
