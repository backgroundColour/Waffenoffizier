#include "ipdialog.h"
#include "QVBoxLayout"

IpDialog::IpDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("IP-Adresse eingeben");

    ipLineEdit = new QLineEdit(this);
    ipLineEdit->setPlaceholderText("Geben Sie die IP-Adresse ein");

    confirmButton = new QPushButton("Bestätigen", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(ipLineEdit);
    layout->addWidget(confirmButton);

    setLayout(layout);

    connect(confirmButton, &QPushButton::clicked, this, [this]() {
        ipAddress = ipLineEdit->text();
        accept();
    });
}

QString IpDialog::getIpAddress() const {
    return ipAddress;
}
