#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsLineItem>
#include <QMediaPlayer>
#include <QLabel> // Include QLabel for displaying text

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showResult(const QString &resultType); // Function to show Positive/Negative result

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QTimer *spawnTimer;
    QGraphicsLineItem *line; // Declare the line variable
    QMediaPlayer *backgroundMusic; // Declare the media player variable
    QLabel *resultLabel; // Label to display Positive/Negative result

    void spawnImage();
};

#endif // MAINWINDOW_H
