#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QObject>

class TcpClient : public QObject {
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    QTcpSocket* socket() const;

    void connectToServer(QString ip);


private:
    QTcpSocket *tcpSocket;
};

#endif // TCPCLIENT_H
