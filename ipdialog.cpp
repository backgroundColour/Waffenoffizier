#include "ipdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "QGuiApplication"
#include "QScreen"
#include "tcpclient.h"

IpDialog::IpDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    tcpClient = new TcpClient(this);


    connect(tcpClient->socket(), &QTcpSocket::connected, this, [this]() {
        confirmButton->setEnabled(true);
        accept();
    });

    // Get screen size
    QSize screenSize = QGuiApplication::primaryScreen()->size();

    resize(screenSize);
    showFullScreen();

    // Background label: set as child, covers entire window
    QLabel *backgroundLabel = new QLabel(this);
    QPixmap bgPixmap(":/new/prefix1/Background.png");

    bgPixmap = bgPixmap.scaled(screenSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    backgroundLabel->setPixmap(bgPixmap);
    backgroundLabel->setGeometry(0, 0, screenSize.width(), screenSize.height());
    backgroundLabel->lower(); // send to back
    backgroundLabel->setAttribute(Qt::WA_TransparentForMouseEvents); // let clicks pass through
    backgroundLabel->show();

    // Container widget for inputs on top of background
    QWidget *container = new QWidget(this);
    container->setGeometry(0, 0, screenSize.width(), screenSize.height());
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->show();

    // Input widgets
    ipLineEdit = new QLineEdit(container);
    ipLineEdit->setPlaceholderText("Geben Sie die IP-Adresse ein");
    ipLineEdit->setStyleSheet("font-size: 24px; padding: 12px; border-radius: 10px; background-color: rgba(255, 255, 255, 200);");

    confirmButton = new QPushButton("Bestätigen", container);
    confirmButton->setStyleSheet("font-size: 24px; padding: 12px; border-radius: 10px; background-color: #d32f2f; color: white;");

    // Layout inside container
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->addStretch();
    layout->addWidget(ipLineEdit);
    layout->addWidget(confirmButton);
    layout->addStretch();
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(300, 0, 300, 0);

    connect(confirmButton, &QPushButton::clicked, this, [this]() {
        QString ip = ipLineEdit->text();
        if (!ip.isEmpty()) {
            confirmButton->setEnabled(false); // Prevent duplicate clicks
            tcpClient->connectToServer(ip);
        }
    });
}

QString IpDialog::getIpAddress() const {
    return ipAddress;
}
