#include "mainwindow.h"
#include <QMenuBar>
#include <QInputDialog>
#include <QDataStream>
#include <QDebug>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _scene = new QGraphicsScene(this);
    _view = new QGraphicsView(_scene, this);
    _view->scale(1, -1);
    setCentralWidget(_view);

    grabKeyboard();

    initdraw();

    _socket = new QTcpSocket(this);
    _packetSize = 0;
    _keys1 = _keys2 = 0;

    connect(_socket, SIGNAL(connected()), this, SLOT(connectedSlot()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    menuBar()->addAction("Connect...", this, SLOT(connectSlot()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::connectSlot()
{
    _socket->connectToHost("localhost", 2222, QIODevice::ReadWrite);
}

void MainWindow::connectedSlot()
{
    qDebug("Connected");
}

void MainWindow::dataReceived()
{
    QDataStream in(_socket);

    if (_packetSize == 0) {
        if (_socket->bytesAvailable() < (qint64)(sizeof (quint16)))
            return;

        in >> _packetSize;
    }
    //    qDebug("Data received %d", _packetSize);

    if (_socket->bytesAvailable() < _packetSize)
        return;

    if (_packetSize == 24) {
        in >> _width;
        in >> _height;
        in >> _netHeight;
        _packetSize = 0;

        // redessiner
        _scene->setSceneRect(0, 0, _width, _height);
        _view->setSceneRect(0, 0, _width, _height);
        _boxItem->setRect(0, 0, _width, _height);
        _netItem->setLine(_width/2.0, 0, _width/2.0, _netHeight);
        redraw();
    }

    if (_packetSize == 52) {
        in >> _ballX;
        in >> _ballY;
        in >> _player1X;
        in >> _player1Y;
        in >> _player2X;
        in >> _player2Y;
        in >> _score1;
        in >> _score2;
        _packetSize = 0;

        // redessiné
        redraw();
        //        qDebug() << _ballX;
        //        qDebug() << _ballY;
        //        qDebug() << _player1X;
        //        qDebug() << _player1Y;
        //        qDebug() << _player2X;
        //        qDebug() << _player2Y;
        //        qDebug() << _score1;
        //        qDebug() << _score2;
    }


}

void MainWindow::initdraw()
{
    _boxItem = _scene->addRect(QRectF(), QPen(QBrush(Qt::black), 2));
    _netItem = _scene->addLine(QLineF(), QPen(QBrush(Qt::black), 3));
    _ballItem = _scene->addEllipse(QRectF(-15, -15, 30, 30), QPen(Qt::black), QBrush(Qt::yellow));
    _slime1Item = _scene->addEllipse(QRectF(-30, -30, 60, 60), QPen(Qt::black), QBrush(Qt::red));
    _slime2Item = _scene->addEllipse(QRectF(-30, -30, 60, 60), QPen(Qt::black), QBrush(Qt::blue));
}

void MainWindow::redraw()
{
    _ballItem->setPos(_ballX, _ballY);
    _slime1Item->setPos(_player1X, _player1Y);
    _slime2Item->setPos(_player2X, _player2Y);

    _scene->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    bool send1 = false;
    bool send2 = false;

    switch (event->key()) {
    case Qt::Key_Left:
        _keys1 |= Left;
        send1 = true;
        break;
    case Qt::Key_Right:
        _keys1 |= Right;
        send1 = true;
        break;
    case Qt::Key_Up:
        _keys1 |= Up;
        send1 = true;
        break;
    case Qt::Key_A:
        _keys2 |= Left;
        send2 = true;
        break;
    case Qt::Key_D:
        _keys2 |= Right;
        send2 = true;
        break;
    case Qt::Key_W:
        _keys2 |= Up;
        send2 = true;
        break;
    default:
        break;
    }

    qDebug() << "keys=" << _keys1;

    if (send1) {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)0;
        out << (quint16)_keys1;
        _socket->write(packet);
    }
    if (send2) {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)1;
        out << (quint16)_keys2;
        _socket->write(packet);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    bool send1 = false;
    bool send2 = false;

    switch (event->key()) {
    case Qt::Key_Left:
        _keys1 &= ~Left;
        send1 = true;
        break;
    case Qt::Key_Right:
        _keys1 &= ~Right;
        send1 = true;
        break;
    case Qt::Key_Up:
        _keys1 &= ~Up;
        send1 = true;
        break;
    case Qt::Key_A:
        _keys2 &= ~Left;
        send2 = true;
        break;
    case Qt::Key_D:
        _keys2 &= ~Right;
        send2 = true;
        break;
    case Qt::Key_W:
        _keys2 &= ~Up;
        send2 = true;
        break;
    default:
        break;
    }

    qDebug() << "keys=" << _keys1;

    if (send1) {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)0;
        out << (quint16)_keys1;
        _socket->write(packet);
    }
    if (send2) {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)1;
        out << (quint16)_keys2;
        _socket->write(packet);
    }
}

