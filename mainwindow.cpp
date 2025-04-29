#include "mainwindow.h"
#include "moveableimage.h"
#include <QGraphicsView>
#include <QRandomGenerator>
#include <QScreen>
#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QCursor>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QUrl>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QtMultimedia>
#include "QGraphicsOpacityEffect"
#include <QParallelAnimationGroup>

MainWindow::MainWindow(const QString &ipAddress, QWidget *parent)
    : QMainWindow(parent), ipAddress(ipAddress) {
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "Connected to server!";
        QString message = "#role:WeaponsOfficer\r\n";
        tcpSocket->write(message.toUtf8());
    });

    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("qrc:/new/prefix1/Musik.mp3"));
    audioOutput->setVolume(0.1);
    player->play();


    QTimer *starSpawnTimer = new QTimer(this);
    connect(starSpawnTimer, &QTimer::timeout, this, &MainWindow::spawnStar);
    starSpawnTimer->start(800); // Spawn a star every 800ms

    connect(tcpSocket, &QTcpSocket::readyRead, this, [this]() {
        QByteArray receivedData = tcpSocket->readAll();
        qDebug() << "Received data: " << receivedData;

        QString receivedMessage = QString::fromUtf8(receivedData);
        qDebug() << "Received message: " << receivedMessage;

        if (receivedMessage == "PositiveLootbox"){

            player->setAudioOutput(audioOutput);
            player->setSource(QUrl("qrc:/new/prefix1/GoodNotif.mp3"));
            audioOutput->setVolume(0.5);
            player->play();

            showResult(receivedMessage);
        }else if(receivedMessage == "NegativeLootbox"){

            player->setAudioOutput(audioOutput);
            player->setSource(QUrl("qrc:/new/prefix1/BadNotif.mp3"));
            audioOutput->setVolume(0.5);
            player->play();

            showResult(receivedMessage);

        }else if(receivedMessage.contains("#ammo:")){
            int ammoIndex = receivedMessage.indexOf("#ammo:");
            QString ammoValue = receivedMessage.mid(ammoIndex + 6).trimmed();
            qDebug() << ammoValue;
            updateAmmoDisplay();
            ammo = ammoValue.toInt();
        }else if(receivedMessage.contains("#weapon:")){
            if(receivedMessage.contains("#weapon:true")){
                canFire = 1;
            }else{
                canFire = 0;
            }
        }
    });


    connect(tcpSocket, &QTcpSocket::errorOccurred, this, [](QAbstractSocket::SocketError socketError) {
        qDebug() << "Error:" << socketError;
    });

    tcpSocket->connectToHost(ipAddress, 9999);
    setWindowFlags(Qt::FramelessWindowHint);

    view = new QGraphicsView(this);

    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    scene = new QGraphicsScene(0, 0, screenWidth, screenHeight, this);
    view->setScene(scene);
    setCentralWidget(view);

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    showMaximized();

    QPixmap backgroundPixmap(":/new/prefix1/Background.png");
    if (!backgroundPixmap.isNull()) {
        QGraphicsPixmapItem *background = new QGraphicsPixmapItem(backgroundPixmap.scaled(screenWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        scene->addItem(background);
        background->setZValue(-2);
    }

    QPixmap layerPixmap(":/new/prefix1/layer.png");
    if (!layerPixmap.isNull()) {
        QGraphicsPixmapItem *layer = new QGraphicsPixmapItem(layerPixmap.scaled(screenWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        scene->addItem(layer);
        layer->setZValue(1);
    }

    QPixmap cursorPixmap(":/new/prefix1/cursor.png");
    if (!cursorPixmap.isNull()) {
        int cursorSize = screenWidth / 15;
        cursorPixmap = cursorPixmap.scaled(cursorSize, cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QCursor customCursor(cursorPixmap);
        setCursor(customCursor);
    }

    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &MainWindow::spawnImage);
    spawnTimer->start(1000);

    ammoLabel = new QLabel(this);
    ammoLabel->setStyleSheet("QLabel { color : white; font-size: 60px; font-weight: 1000}");
    updateAmmoDisplay();
    ammoLabel->setGeometry(screenWidth - 450, screenHeight - 250, 400, 230); // Position in the bottom-right corner
    ammoLabel->show();
}

MainWindow::~MainWindow() {
    delete scene;
    delete spawnTimer;
    delete resultLabel;
    delete shakeAnimation;
    delete ammoLabel;
}

void MainWindow::spawnStar() {
    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    QPixmap starPixmap(":/new/prefix1/Star.png");
    if (starPixmap.isNull()) {
        return;
    }

    int size = QRandomGenerator::global()->bounded(20, 50);
    starPixmap = starPixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int x = QRandomGenerator::global()->bounded(0, screenWidth - size);
    int y = QRandomGenerator::global()->bounded(0, screenHeight - size);

    QGraphicsPixmapItem *star = new QGraphicsPixmapItem(starPixmap);
    star->setPos(x, y);
    star->setZValue(-1);
    scene->addItem(star);
    stars.append(star);

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    star->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(QRandomGenerator::global()->bounded(1000, 3000));
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(QRandomGenerator::global()->bounded(5, 10) / 10.0); // Random opacity between 0.5 and 1.0

    QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(QRandomGenerator::global()->bounded(1000, 3000));
    fadeOut->setStartValue(fadeIn->endValue());
    fadeOut->setEndValue(0);

    QSequentialAnimationGroup *animationGroup = new QSequentialAnimationGroup(this);
    animationGroup->addAnimation(fadeIn);
    animationGroup->addPause(QRandomGenerator::global()->bounded(2000, 5000)); // Stay visible for random time
    animationGroup->addAnimation(fadeOut);

    connect(animationGroup, &QSequentialAnimationGroup::finished, [this, star]() {
        scene->removeItem(star);
        stars.removeOne(star);
        delete star;
    });

    animationGroup->start();
}

void MainWindow::updateAmmoDisplay() {
    ammoLabel->setText("Munition " + QString::number(ammo));

    if (ammo == 0) {
        ammoLabel->setStyleSheet("QLabel { color : Red; font-size: 60px; font-weight: 1000}");
    }else{
        ammoLabel->setStyleSheet("QLabel { color : White; font-size: 60px; font-weight: 1000}");
    }
}

bool MainWindow::canShoot() {

    if (canFire == 0) {
        return false;
    }

    return ammo > 0;
}

void MainWindow::fireEvent() {

    if (ammo == 0 || canFire == 0) {
        player->setAudioOutput(audioOutput);
        player->setSource(QUrl("qrc:/new/prefix1/Empty.mp3"));
        audioOutput->setVolume(0.5);
        player->play();
    }else{
        writeMessage(QString("#shoot:none"));
    }

    if (canShoot()) {
        updateAmmoDisplay();
        if (ammo == 0) {
            player->setAudioOutput(audioOutput);
            player->setSource(QUrl("qrc:/new/prefix1/AmmoEmptied.mp3"));
            audioOutput->setVolume(0.5);
            player->play();
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    fireEvent();
}

void MainWindow::writeMessage(QString message) {
    tcpSocket->write(message.toUtf8());
}

void MainWindow::shakeContent() {
    if (!shakeAnimation) {
        shakeAnimation = new QPropertyAnimation(view, "pos"); // Animate the QGraphicsView's position
        shakeAnimation->setDuration(100); // Duration of the shake
        shakeAnimation->setLoopCount(2); // Number of shakes
    }

    // Store the original position of the QGraphicsView
    QPoint originalPos = view->pos();

    // Define the shake animation keyframes
    shakeAnimation->setKeyValueAt(0, originalPos);
    shakeAnimation->setKeyValueAt(0.1, originalPos + QPoint(-10, 0));
    shakeAnimation->setKeyValueAt(0.2, originalPos + QPoint(10, 0));
    shakeAnimation->setKeyValueAt(0.3, originalPos + QPoint(-10, 0));
    shakeAnimation->setKeyValueAt(0.4, originalPos + QPoint(10, 0));
    shakeAnimation->setKeyValueAt(0.5, originalPos + QPoint(-10, 0));
    shakeAnimation->setKeyValueAt(0.6, originalPos + QPoint(10, 0));
    shakeAnimation->setKeyValueAt(0.7, originalPos + QPoint(-10, 0));
    shakeAnimation->setKeyValueAt(0.8, originalPos + QPoint(10, 0));
    shakeAnimation->setKeyValueAt(0.9, originalPos + QPoint(-10, 0));
    shakeAnimation->setKeyValueAt(1, originalPos);

    // Start the animation
    shakeAnimation->start();
}


void MainWindow::spawnImage() {
    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    int width = QRandomGenerator::global()->bounded(screenWidth / 10, screenWidth / 5);
    int height = QRandomGenerator::global()->bounded(screenHeight / 10, screenHeight / 5);

    int randomNumber = QRandomGenerator::global()->bounded(1, 101);
    QPixmap pixmap;
    QString type;

    if(randomNumber < 70){
        pixmap = QPixmap(":/new/prefix1/meteor.png");
        type = "meteor";
    }else{
        randomNumber = QRandomGenerator::global()->bounded(1, 101);


        if (randomNumber < 65) {
            randomNumber = QRandomGenerator::global()->bounded(1, 101);

            if (randomNumber < 50) {
                pixmap = QPixmap(":/new/prefix1/Raumschiff1.png");
                type = "BlueShip";
            }else{
                pixmap = QPixmap(":/new/prefix1/Raumschiff2.png");
                type = "RedShip";
            }
        }else{
            pixmap = QPixmap(":/new/prefix1/Lootbox.png");
            type = "Lootbox";
        }
    }

    if (pixmap.isNull()) {
        return;
    }

    pixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int y = QRandomGenerator::global()->bounded(50, screenHeight - height - 50);
    int x = screenWidth + 50;

    MovableImage *img = new MovableImage(pixmap, x, y, type, this);

    if (type != "BlueShip" && type != "RedShip") {
        qreal rotationAngle = QRandomGenerator::global()->bounded(-45, 45);
        img->setRotation(rotationAngle);
    }

    img->setZValue(0);
    scene->addItem(img);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::showResult(const QString &resultType) {
    if (resultLabel) {
        resultLabel->hide();
        delete resultLabel;
        resultLabel = nullptr;
    }

    QPixmap resultPixmap(resultType == "Positive" ? ":/new/prefix1/Positive.png" : ":/new/prefix1/Negative.png");
    if (resultPixmap.isNull()) {
        return;
    }

    resultLabel = new QLabel(this);

    resultPixmap = resultPixmap.scaled(200, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QImage imageWithText(resultPixmap.size(), QImage::Format_ARGB32);
    imageWithText.fill(Qt::transparent);

    QPainter painter(&imageWithText);
    painter.drawPixmap(0, 0, resultPixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(imageWithText.rect(), Qt::AlignCenter, resultType);
    painter.end();

    resultLabel->setPixmap(QPixmap::fromImage(imageWithText));
    resultLabel->setAlignment(Qt::AlignCenter);

    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    resultLabel->setGeometry(
        20,
        screenGeometry.height() - 120,
        200,
        100
        );

    resultLabel->setCursor(Qt::ArrowCursor);

    resultLabel->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    resultLabel->setAttribute(Qt::WA_ShowWithoutActivating, false);
    resultLabel->setStyleSheet("background: rgba(0, 0, 0, 0.7); border: 2px solid white;");
    resultLabel->show();

    QTimer::singleShot(2000, [this]() {
        if (resultLabel) {
            resultLabel->hide();
            delete resultLabel;
            resultLabel = nullptr;
        }
    });

    if (resultType == "Negative") {
        shakeContent();
    }
}
