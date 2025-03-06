#include "moveableimage.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <QDebug>
#include "QRandomGenerator"
#include "mainwindow.h"
#include "QtMultimedia"

QMediaPlayer *player = new QMediaPlayer;
QAudioOutput *audioOutput = new QAudioOutput;

MovableImage::MovableImage(const QPixmap &pixmap, int startX, int startY, const QString &type, MainWindow *mainWindow)
    : QGraphicsPixmapItem(pixmap), speed(50), animation(nullptr), currentFrame(0), type(type), mainWindow(mainWindow) {
    setPos(startX, startY);

    // Timer to move the image
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &MovableImage::move);
    moveTimer->start(50); // Adjust the interval to control movement smoothness
}

void MovableImage::move() {
    // Move the item left by "speed" pixels
    setPos(x() - speed, y());

    // Check if the item is out of the scene (left side)
    if (x() + pixmap().width() < 0) {
        // Remove from the scene and delete the item
        scene()->removeItem(this);
        delete this;
    }
}

void MovableImage::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // Stop the movement
    if (moveTimer) {
        moveTimer->stop();
    }

    // Play the animation
    playAnimation();

    // Check the type of image that was clicked
    if (type == "meteor") {
        type = "AHHH";
        qDebug() << "Meteor image was clicked.";


        player->setAudioOutput(audioOutput);
        player->setSource(QUrl("qrc:/new/prefix1/durchfall.mp3"));
        audioOutput->setVolume(0.5);  // Adjust volume if needed
        player->play();

    } else if (type == "raumschiff1") {
        type = "AHHH";
        qDebug() << "Raumschiff1 image was clicked.";
    } else if (type == "raumschiff2") {
        type = "AHHH";
        qDebug() << "Raumschiff2 image was clicked.";
    } else if (type == "Lootbox") {
        type = "AHHH";
        qDebug() << "Lootbox image was clicked.";
        // Randomly decide Positive or Negative
        QString resultType = (QRandomGenerator::global()->bounded(2) == 0) ? "Positive" : "Negative";
        if (mainWindow) {
            mainWindow->showResult(resultType);
        }
    }
}

void MovableImage::playAnimation() {
    QStringList frames = {":/new/prefix1/stage1.png", ":/new/prefix1/stage2.png", ":/new/prefix1/stage3.png"};

    if (currentFrame < frames.size()) {
        QPixmap framePixmap(frames[currentFrame]);
        if (!framePixmap.isNull()) {
            framePixmap = framePixmap.scaled(pixmap().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // Ensure the frame is scaled properly
            setPixmap(framePixmap);

            // Center the animation image on the original position
            QPointF currentPosition = pos();
            QPointF centerOffset((pixmap().width() - framePixmap.width()) / 2, (pixmap().height() - framePixmap.height()) / 2);
            setPos(currentPosition + centerOffset);
        }
        currentFrame++;
        QTimer::singleShot(100, this, &MovableImage::playAnimation); // Play next frame after 200 ms
    } else {
        // Animation complete, remove the item
        scene()->removeItem(this);
        delete this;
    }
}
