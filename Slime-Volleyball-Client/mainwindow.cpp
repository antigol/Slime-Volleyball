#include "mainwindow.h"
#include <QMenuBar>
#include <QInputDialog>
#include <QDataStream>
#include <QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _scene = new QGraphicsScene(this);
    _view = new QGraphicsView(_scene, this);
    _view->scale(1, -1);

    setCentralWidget(_view);

    initdraw();

    _socket = new QTcpSocket(this);
    _packetSize = 0;
    _keys1 = _keys2 = 0;

    connect(_socket, SIGNAL(connected()), this, SLOT(connectedSlot()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    QMenu *menu = menuBar()->addMenu("&Start");
    menu->addAction("&Connect...", this, SLOT(connectSlot()));

    QMenu *menuPlayers = menuBar()->addMenu("Players");
    menuPlayers->addAction("Select player...", this, SLOT(selectPlayer()));

    connect(&_timer, SIGNAL(timeout()), SLOT(timerSlot()));

    _timer.start(20);
}

MainWindow::~MainWindow()
{

}

void MainWindow::connectSlot()
{
    QSettings set;

    _socket->abort();
    QString srv = QInputDialog::getText(this, "server", "server", QLineEdit::Normal, set.value("url", "localhost").toString());
    if (!srv.isEmpty()) {
        set.setValue("url", srv);
        _socket->connectToHost(srv, 2424, QIODevice::ReadWrite);
    }
}

void MainWindow::connectedSlot()
{
    qDebug("Connected");
}

void MainWindow::errorSlot(QAbstractSocket::SocketError )
{
    qDebug() << _socket->errorString();
}

void MainWindow::dataReceived()
{
    QDataStream in(_socket);

    if (_packetSize == 0) {
        if (_socket->bytesAvailable() < (qint64)(sizeof (quint8)))
            return;

        in >> _packetId;

        switch (_packetId) {
        case 1:
            _packetSize = 48;
            break;
        case 2:
            _packetSize = 52;
            break;
        default:
            _packetSize = 0;
            _socket->readAll();
            break;
        }
    }

    if (_socket->bytesAvailable() < _packetSize)
        return;

    if (_socket->bytesAvailable() > _packetSize) {
        qDebug() << "more than packet size, skip data... (" << _socket->bytesAvailable() << ")";
        in.skipRawData(_packetSize);
        _packetSize = 0;
        dataReceived();
        return;
    }


    if (_packetId == 1) {
        in >> _width;
        in >> _height;
        in >> _netHeight;
        in >> _ballradius;
        in >> _slimeradius;
        in >> _playerSpeed;
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

    if (_packetId == 2) {
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        in >> _ball.rx();
        in >> _ball.ry();
        in >> _player1.rx();
        in >> _player1.ry();
        in >> _player2.rx();
        in >> _player2.ry();
        in >> _ballSpeed.rx();
        in >> _ballSpeed.ry();
        in >> _player1Speed.rx();
        in >> _player1Speed.ry();
        in >> _player2Speed.rx();
        in >> _player2Speed.ry();
        in >> _score1;
        in >> _score2;
        _packetSize = 0;

        _drawMutex.lock();
        _timer.start();
        _time.restart();
        redraw();
        _drawMutex.unlock();
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
    _ballItem->setPos(_ball);
    _slime1Item->setPos(_player1);
    _slime2Item->setPos(_player2);

    _scene->update();
}

void MainWindow::timerSlot()
{
    double dt;
    if (_drawMutex.tryLock()) {
        dt = (double)_time.restart() / 1000.0;
        _ball += _ballSpeed * dt;
        _player1 += _player1Speed * dt;
        _player2 += _player2Speed * dt;
        redraw();
        _drawMutex.unlock();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    quint16 k1 = _keys1;
    quint16 k2 = _keys2;

    switch (event->key()) {
    case Qt::Key_J:
        qDebug() << "Left Press";
        k2 |= Left;
        k2 &= ~Right;
        //        _player2Speed.rx() = -_playerSpeed;
        break;
    case Qt::Key_L:
        qDebug() << "Right Press";
        k2 |= Right;
        k2 &= ~Left;
        //        _player2Speed.rx() = +_playerSpeed;
        break;
    case Qt::Key_I:
        k2 |= Up;
        break;
    case Qt::Key_A:
        k1 |= Left;
        k1 &= ~Right;
        //        _player1Speed.rx() = -_playerSpeed;
        break;
    case Qt::Key_D:
        k1 |= Right;
        k1 &= ~Left;
        //        _player1Speed.rx() = +_playerSpeed;
        break;
    case Qt::Key_W:
        k1 |= Up;
        break;
    default:
        break;
    }

    if (k1 != _keys1 || k2 != _keys2) {
        _keys1 = k1;
        _keys2 = k2;
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)(_player==2?0x8:_keys1);
        out << (quint16)(_player==1?0x8:_keys2);
        _socket->write(packet);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    quint16 k1 = _keys1;
    quint16 k2 = _keys2;

    switch (event->key()) {
    case Qt::Key_J:
        qDebug() << "Right Release";
        k2 &= ~Left;
        //        _player2Speed.rx() = 0;
        break;
    case Qt::Key_L:
        qDebug() << "Right Release";
        k2 &= ~Right;
        //        _player2Speed.rx() = 0;
        break;
    case Qt::Key_I:
        k2 &= ~Up;
        break;
    case Qt::Key_A:
        k1 &= ~Left;
        //        _player1Speed.rx() = 0;
        break;
    case Qt::Key_D:
        k1 &= ~Right;
        //        _player1Speed.rx() = 0;
        break;
    case Qt::Key_W:
        k1 &= ~Up;
        break;
    default:
        break;
    }

    if (k1 != _keys1 || k2 != _keys2) {
        _keys1 = k1;
        _keys2 = k2;
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)(_player==2?0x8:_keys1);
        out << (quint16)(_player==1?0x8:_keys2);
        _socket->write(packet);
    }
}

void MainWindow::selectPlayer()
{
    _player = QInputDialog::getInt(this, "player", "1=player1, 2=player2, 0=both");
}



