#ifndef IPDIALOG_H
#define IPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class IpDialog : public QDialog {
    Q_OBJECT

public:
    explicit IpDialog(QWidget *parent = nullptr);
    QString getIpAddress() const;

private:
    QLineEdit *ipLineEdit;
    QPushButton *confirmButton;
    QString ipAddress;
};

#endif // IPDIALOG_H
