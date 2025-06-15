#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class server : public QObject {
public:
    server(QObject* parent = nullptr);
private slots:
    void newConnection();  // Called when a client connects
    void readData();       // Called when data is available to read
private:
    QTcpServer *TCPServer;
    QTcpSocket *clientSocket;
};

#endif // SERVER_H
