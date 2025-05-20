#include "tcpclient.h"
#include <QEventLoop>
#include <QTimer>

TcpClient::TcpClient(QObject *parent)
    : QObject(parent), tcpSocket(new QTcpSocket(this)) {}

void TcpClient::connectToServer(QString ip)
{
    tcpSocket->connectToHost(ip,9999);

}


QTcpSocket* TcpClient::socket() const {
    return tcpSocket;
}
