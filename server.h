#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject {
public:
    Server(const QString& roomName, QObject* parent = nullptr);
private slots:
    void newConnection();  // Called when a client connects
    void readData();       // Called when data is available to read

signals:
    void serverStarted(const QString& roomName, int port);
    void serverError(const QString& errorMessage);
    void clientConnected(const QString& username);
    void clientDisconnected(const QString& username);
    void dataReceived(const QString& username, const QByteArray& message);

private:
    QString roomName;
    QTcpServer *TCPServer;
    QTcpSocket *clientSocket;

};

#endif // SERVER_H
