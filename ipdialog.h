#ifndef IPDIALOG_H
#define IPDIALOG_H
#include "tcpclient.h"

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "QGraphicsScene"

class IpDialog : public QDialog {
    Q_OBJECT

public:
    explicit IpDialog(QWidget *parent = nullptr);
    QString getIpAddress() const;

private:
    QLineEdit *ipLineEdit;
    QPushButton *confirmButton;
    QString ipAddress;
    QGraphicsScene *scene;
    QGraphicsView *view;
    TcpClient* tcpClient;
};

#endif // IPDIALOG_H
