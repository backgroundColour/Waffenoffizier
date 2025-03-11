#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsLineItem>
#include <QMediaPlayer>
#include <QLabel>
#include <QPropertyAnimation>
#include "QGraphicsView"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showResult(const QString &resultType);
    void shakeContent();
    void updateAmmoDisplay();
    bool canShoot();
    void fireEvent();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *spawnTimer;
    QGraphicsLineItem *line;
    QMediaPlayer *backgroundMusic;
    QLabel *resultLabel;
    QPropertyAnimation *shakeAnimation;
    QLabel *ammoLabel;
    int ammo;

    void spawnImage();
};

#endif
