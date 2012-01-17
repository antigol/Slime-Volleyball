#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTcpSocket>
#include <QTime>
#include <QMutex>
#include <QTimer>

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum Movements {
        DontMove = 0x0,
        Left = 0x1,
        Right = 0x2,
        Up = 0x4
    };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void connectSlot();
    void connectedSlot();
    void errorSlot(QAbstractSocket::SocketError error);
    void dataReceived();
    void timerSlot();

private:
    void initdraw();
    void redraw();

    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    QGraphicsView *_view;
    QGraphicsScene *_scene;
    QTcpSocket *_socket;

    quint8 _packetId;
    quint16 _packetSize;

    double _width;
    double _height;
    double _netHeight;
    double _ballradius;
    double _slimeradius;
    double _playerSpeed;

    QPointF _ball;
    QPointF _player1;
    QPointF _player2;
    QPointF _ballSpeed;
    QPointF _player1Speed;
    QPointF _player2Speed;
    quint16 _score1;
    quint16 _score2;

    QGraphicsEllipseItem *_ballItem;
    QGraphicsEllipseItem *_slime1Item;
    QGraphicsEllipseItem *_slime2Item;
    QGraphicsLineItem *_netItem;
    QGraphicsRectItem *_boxItem;

    quint16 _keys1;
    quint16 _keys2;
    QTime _time;
    QMutex _drawMutex;
    QTimer _timer;
};

#endif // MAINWINDOW_H
