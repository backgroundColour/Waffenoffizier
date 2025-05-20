#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsLineItem>
#include <QMediaPlayer>
#include <QLabel>
#include <QPropertyAnimation>
#include "QGraphicsView"
#include "qaudiooutput.h"
#include <QTcpSocket>
#include "tcpclient.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const QString &ipAddress, QWidget *parent = nullptr);
    ~MainWindow();
    void showResult(const QString &resultType);
    void shakeContent();
    void updateAmmoDisplay();
    bool canShoot();
    void fireEvent();
    void writeMessage(const QString message);
    void gameOver();
    void useTcpSocket();

    int getCanFire() const;
    void setCanFire(int newCanFire);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMediaPlayer *player = new QMediaPlayer;
    QAudioOutput *audioOutput = new QAudioOutput;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *spawnTimer;
    QGraphicsLineItem *line;
    QMediaPlayer *backgroundMusic;
    QLabel *resultLabel;
    QPropertyAnimation *shakeAnimation = NULL;
    QLabel *ammoLabel;
    QString ipAddress;
    QList<QGraphicsPixmapItem*> stars;
    QTcpSocket *tcpSocket;
    //TcpClient* tcpClient;

    void spawnStar();
    void spawnImage();

    int ammo = 50;
    int canFire = 1;
};

#endif
