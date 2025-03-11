#ifndef MOVABLEIMAGE_H
#define MOVABLEIMAGE_H

#include "QPropertyAnimation"
#include <QGraphicsPixmapItem>
#include <QTimer>

class MainWindow;

class MovableImage : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    MovableImage(const QPixmap &pixmap, int startX, int startY, const QString &type, MainWindow *mainWindow);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
    void move();
    void playAnimation();

private:
    QTimer *moveTimer;
    int speed;
    int currentFrame;
    QPropertyAnimation *animation;
    QString type;
    MainWindow *mainWindow;
};

#endif
