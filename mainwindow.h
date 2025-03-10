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
    void showResult(const QString &resultType); // Function to show Positive/Negative result
    void shakeContent(); // Shake the content of the window (QGraphicsView)

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsView *view; // Add a pointer to the QGraphicsView
    QTimer *spawnTimer;
    QGraphicsLineItem *line;
    QMediaPlayer *backgroundMusic;
    QLabel *resultLabel;
    QPropertyAnimation *shakeAnimation; // Animation for shaking effect

    void spawnImage();
};

#endif // MAINWINDOW_H
