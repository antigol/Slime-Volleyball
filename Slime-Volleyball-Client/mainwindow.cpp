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

    startTimer(20);
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

    if (_packetSize == 40) {
        in >> _width;
        in >> _height;
        in >> _netHeight;
        in >> _ballradius;
        in >> _slimeradius;
        _packetSize = 0;

        setGeometry(QRect(pos(), QSize(_width, _height) * 1.2));
        _scene->setSceneRect(0, 0, _width, _height);
        _view->setSceneRect(0, 0, _width, _height);
        _boxItem->setRect(0, 0, _width, _height);
        _netItem->setLine(_width/2.0, 0, _width/2.0, _netHeight);
        _ballItem->setRect(-_ballradius, -_ballradius, 2.0*_ballradius, 2.0*_ballradius);
        _slime1Item->setRect(-_slimeradius, -_slimeradius, 2.0*_slimeradius, 2.0*_slimeradius);
        _slime2Item->setRect(-_slimeradius, -_slimeradius, 2.0*_slimeradius, 2.0*_slimeradius);
    }

    if (_packetSize == 52) {
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        in >> _ballX;
        in >> _ballY;
        in >> _player1X;
        in >> _player1Y;
        in >> _player2X;
        in >> _player2Y;
        in.skipRawData(sizeof (float) * 6);
        in >> _score1;
        in >> _score2;
        _packetSize = 0;

        qDebug() << _score1 << " à " << _score2 << ".";
        //        qDebug() << _ballX;
        //        qDebug() << _ballY;
        //        qDebug() << _player1X;
        //        qDebug() << _player1Y;
        //        qDebug() << _player2X;
        //        qDebug() << _player2Y;
        //        qDebug() << _score1;
        //        qDebug() << _score2;
    }

    if (_socket->bytesAvailable() > _packetSize) {
        qDebug() << "sa va trop vite !!";
        dataReceived();
    }
}

void MainWindow::initdraw()
{
    _boxItem = _scene->addRect(QRectF(), QPen(QBrush(Qt::black), 2));
    _netItem = _scene->addLine(QLineF(), QPen(QBrush(Qt::black), 3));
    _ballItem = _scene->addEllipse(QRectF(), QPen(Qt::black), QBrush(Qt::yellow));
    _slime1Item = _scene->addEllipse(QRectF(), QPen(Qt::black), QBrush(Qt::red));
    _slime2Item = _scene->addEllipse(QRectF(), QPen(Qt::black), QBrush(Qt::blue));
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

    switch (event->key()) {
    case Qt::Key_Left:
        _keys2 |= Left;
        break;
    case Qt::Key_Right:
        _keys2 |= Right;
        break;
    case Qt::Key_Up:
        _keys2 |= Up;
        break;
    case Qt::Key_A:
        _keys1 |= Left;
        break;
    case Qt::Key_D:
        _keys1 |= Right;
        break;
    case Qt::Key_W:
        _keys1 |= Up;
        break;
    default:
        break;
    }

    qDebug() << "keys=" << _keys1;

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16)_keys1;
    out << (quint16)_keys2;
    _socket->write(packet);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_Left:
        _keys2 &= ~Left;
        break;
    case Qt::Key_Right:
        _keys2 &= ~Right;
        break;
    case Qt::Key_Up:
        _keys2 &= ~Up;
        break;
    case Qt::Key_A:
        _keys1 &= ~Left;
        break;
    case Qt::Key_D:
        _keys1 &= ~Right;
        break;
    case Qt::Key_W:
        _keys1 &= ~Up;
        break;
    default:
        break;
    }

    qDebug() << "keys=" << _keys1;

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16)_keys1;
    out << (quint16)_keys2;
    _socket->write(packet);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    redraw();
}

