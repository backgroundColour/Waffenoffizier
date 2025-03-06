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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), line(nullptr), resultLabel(nullptr) {
    // Set the window to be borderless
    setWindowFlags(Qt::FramelessWindowHint);

    QGraphicsView *view = new QGraphicsView(this);

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
    if (backgroundPixmap.isNull()) {
        qDebug() << "Failed to load background image!";
    } else {
        QGraphicsPixmapItem *background = new QGraphicsPixmapItem(backgroundPixmap.scaled(screenWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        scene->addItem(background);
        background->setZValue(-2); // Ensure background is behind other items
    }

    // Load and set layer image
    QPixmap layerPixmap(":/new/prefix1/layer.png");
    if (layerPixmap.isNull()) {
        qDebug() << "Failed to load layer image!";
    } else {
        QGraphicsPixmapItem *layer = new QGraphicsPixmapItem(layerPixmap.scaled(screenWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        scene->addItem(layer);
        layer->setZValue(1); // Ensure layer is above other items
    }

    // Load and set custom cursor
    QPixmap cursorPixmap(":/new/prefix1/cursor.png");
    if (cursorPixmap.isNull()) {
        qDebug() << "Failed to load cursor image!";
    } else {
        int cursorSize = screenWidth / 15; // Increase size to make it larger
        cursorPixmap = cursorPixmap.scaled(cursorSize, cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QCursor customCursor(cursorPixmap);
        setCursor(customCursor);
    }

    // Create and add the line item
    line = new QGraphicsLineItem();
    scene->addItem(line);
    line->setZValue(2); // Ensure line is above other items

    // Timer to spawn images every 2 seconds
    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &MainWindow::spawnImage);
    spawnTimer->start(1000);
}

MainWindow::~MainWindow() {
    delete scene;
    delete spawnTimer;
    delete line;
    delete resultLabel;
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

        qDebug() << randomNumber;

        if (randomNumber < 65) {
            randomNumber = QRandomGenerator::global()->bounded(1, 101);

            if (randomNumber < 50) {
                pixmap = QPixmap(":/new/prefix1/Raumschiff1.png");
                type = "raumschiff1";
            }else{
                pixmap = QPixmap(":/new/prefix1/Raumschiff2.png");
                type = "raumschiff2";
            }
        }else{
            pixmap = QPixmap(":/new/prefix1/Lootbox.png");
            type = "Lootbox";
        }
    }

    if (pixmap.isNull()) {
        qDebug() << "Failed to load image from resource!";
        return;
    }

    pixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Randomize vertical position and fix horizontal position to spawn off-screen
    int y = QRandomGenerator::global()->bounded(50, screenHeight - height - 50);
    int x = screenWidth + 50; // Spawn off-screen to the right

    MovableImage *img = new MovableImage(pixmap, x, y, type, this);

    // Randomly rotate the image if it's not Raumschiff1 or Raumschiff2
    if (type != "raumschiff1" && type != "raumschiff2") {
        qreal rotationAngle = QRandomGenerator::global()->bounded(-45, 45); // Rotate between -45 and 45 degrees
        img->setRotation(rotationAngle);
    }

    img->setZValue(0); // Ensure meteors are below the layer
    scene->addItem(img);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (line) {
        QRect screenGeometry = QApplication::screens().at(0)->geometry();
        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        // Update the line to follow the cursor
        QPointF cursorPosition = event->pos();
        QLineF lineToCursor(screenWidth / 2, screenHeight, cursorPosition.x(), cursorPosition.y());
        line->setLine(lineToCursor);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::showResult(const QString &resultType) {
    // If a previous result is already displayed, remove it
    if (resultLabel) {
        resultLabel->hide();
        delete resultLabel;
        resultLabel = nullptr;
    }

    // Load the appropriate image
    QPixmap resultPixmap(resultType == "Positive" ? ":/new/prefix1/Positive.png" : ":/new/prefix1/Negative.png");
    if (resultPixmap.isNull()) {
        qDebug() << "Failed to load result image!";
        return;
    }

    // Create a new label to display the result
    resultLabel = new QLabel(this);

    // Make the pixmap smaller
    resultPixmap = resultPixmap.scaled(200, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation); // Smaller size
    QImage imageWithText(resultPixmap.size(), QImage::Format_ARGB32);
    imageWithText.fill(Qt::transparent); // Transparent background for overlay

    // Paint placeholder text on the image
    QPainter painter(&imageWithText);
    painter.drawPixmap(0, 0, resultPixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold)); // Smaller font for smaller box
    painter.drawText(imageWithText.rect(), Qt::AlignCenter, resultType); // Placeholder text in the middle
    painter.end();

    // Set the image with text to the label
    resultLabel->setPixmap(QPixmap::fromImage(imageWithText));
    resultLabel->setAlignment(Qt::AlignCenter);

    // Position the label in the bottom-left corner
    QRect screenGeometry = QApplication::screens().at(0)->geometry();
    resultLabel->setGeometry(
        20,                             // 20px margin from the left edge
        screenGeometry.height() - 120,  // 20px margin from the bottom edge
        200,                            // Width of the label
        100                             // Height of the label
        );

    // Fix the cursor behavior
    resultLabel->setCursor(Qt::ArrowCursor);

    // Make the label modal and block interactions with the underlying UI
    resultLabel->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    resultLabel->setAttribute(Qt::WA_ShowWithoutActivating, false); // Prevent clicks from passing through
    resultLabel->setStyleSheet("background: rgba(0, 0, 0, 0.7); border: 2px solid white;"); // Optional style for visibility
    resultLabel->show();

    // Set a timer to hide the label after 2 seconds
    QTimer::singleShot(2000, [this]() {
        if (resultLabel) {
            resultLabel->hide();
            delete resultLabel;
            resultLabel = nullptr;
        }
    });
}
