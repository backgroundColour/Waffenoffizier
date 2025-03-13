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
#include <QPropertyAnimation> // Include QPropertyAnimation for shaking effect
#include <QtMultimedia>




MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), resultLabel(nullptr), shakeAnimation(nullptr), ammo(50) {
    // Set the window to be borderless
    setWindowFlags(Qt::FramelessWindowHint);

    view = new QGraphicsView(this); // Store the QGraphicsView in the class

    // Set scene to match window size
    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    scene = new QGraphicsScene(0, 0, screenWidth, screenHeight, this);
    view->setScene(scene);
    setCentralWidget(view);

    // Disable scroll bars
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Maximize the window
    showMaximized();

    // Load and set background image
    QPixmap backgroundPixmap(":/new/prefix1/Background.png");
    if (!backgroundPixmap.isNull()) {
        QGraphicsPixmapItem *background = new QGraphicsPixmapItem(backgroundPixmap.scaled(screenWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        scene->addItem(background);
        background->setZValue(-2); // Ensure background is behind other items
    }

    // Load and set layer image
    QPixmap layerPixmap(":/new/prefix1/layer.png");
    if (!layerPixmap.isNull()) {
        QGraphicsPixmapItem *layer = new QGraphicsPixmapItem(layerPixmap.scaled(screenWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        scene->addItem(layer);
        layer->setZValue(1); // Ensure layer is above other items
    }

    // Load and set custom cursor
    QPixmap cursorPixmap(":/new/prefix1/cursor.png");
    if (!cursorPixmap.isNull()) {
        int cursorSize = screenWidth / 15; // Increase size to make it larger
        cursorPixmap = cursorPixmap.scaled(cursorSize, cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QCursor customCursor(cursorPixmap);
        setCursor(customCursor);
    }

    // Timer to spawn images every 2 seconds
    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &MainWindow::spawnImage);
    spawnTimer->start(1000);

    // Initialize ammo display
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

void MainWindow::updateAmmoDisplay() {
    ammoLabel->setText("Munition " + QString::number(ammo));

    if (ammo == 0) {
        ammoLabel->setStyleSheet("QLabel { color : Red; font-size: 60px; font-weight: 1000}");
    }else{
        ammoLabel->setStyleSheet("QLabel { color : White; font-size: 60px; font-weight: 1000}");
    }
}

bool MainWindow::canShoot() {
    return ammo > 0;
}

void MainWindow::fireEvent() {

    if (ammo == 0) {
        player->setAudioOutput(audioOutput);
        player->setSource(QUrl("qrc:/new/prefix1/Empty.mp3"));
        audioOutput->setVolume(0.5);
        player->play();
    }

    if (canShoot()) {
        ammo--;
        updateAmmoDisplay();

        qDebug() << ammo;

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
    // Get screen resolution
    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    // Randomize image size
    int width = QRandomGenerator::global()->bounded(screenWidth / 10, screenWidth / 5);
    int height = QRandomGenerator::global()->bounded(screenHeight / 10, screenHeight / 5);

    // Load and scale the image
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

    resultPixmap = resultPixmap.scaled(200, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation); // Smaller size
    QImage imageWithText(resultPixmap.size(), QImage::Format_ARGB32);
    imageWithText.fill(Qt::transparent);

    QPainter painter(&imageWithText);
    painter.drawPixmap(0, 0, resultPixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold)); // Smaller font for smaller box
    painter.drawText(imageWithText.rect(), Qt::AlignCenter, resultType); // Placeholder text in the middle
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
    resultLabel->setAttribute(Qt::WA_ShowWithoutActivating, false); // Prevent clicks from passing through
    resultLabel->setStyleSheet("background: rgba(0, 0, 0, 0.7); border: 2px solid white;"); // Optional style for visibility
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
