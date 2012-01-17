#include "server.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>

Server::Server(QObject *parent) :
    QObject(parent)
{
    _world = new World();

    _server = new QTcpServer(this);

    _timerId = 0;
    _playersKeys[0] = _playersKeys[1] = World::DontMove;
}

Server::~Server()
{
    stop();
    delete _world;
}

bool Server::play()
{
    if (_timerId != 0)
        stop();

    // initialisation
    if (_server->listen(QHostAddress::Any, _portNumber)) {
        qDebug() << "server start to listen on port : " << _portNumber;
        connect(_server, SIGNAL(newConnection()), this, SLOT(newClient()));
    } else {
        qDebug() << "server can't start on port : " << _portNumber;
        return false;
    }

    world()->reset();

    _time.start();
    _timerId = startTimer(50);

    return (_timerId != 0);
}

void Server::stop()
{
    // arrêt
    _stopServer = true;
    //    wait();
    killTimer(_timerId);
    _timerId = 0;
    _server->close();
    qDebug("Server stopped");
}

void Server::timerEvent(QTimerEvent *)
{
    _runMutex.lock();
    double dt = (double)_time.restart() / 1000.0;
    _world->exactMove(dt, _playersKeys);
    _runMutex.unlock();

    // Création du paquet {positions et vitesse}
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);
    out << (quint8)2;
    out << _world->_ballActualPos.x();
    out << _world->_ballActualPos.y();
    out << _world->_playersActualPos[0].x();
    out << _world->_playersActualPos[0].y();
    out << _world->_playersActualPos[1].x();
    out << _world->_playersActualPos[1].y();
    out << _world->_ballActualSpeed.x();
    out << _world->_ballActualSpeed.y();
    out << _world->_playersActualSpeed[0].x();
    out << _world->_playersActualSpeed[0].y();
    out << _world->_playersActualSpeed[1].x();
    out << _world->_playersActualSpeed[1].y();
    out << (quint16)_world->_score[0];
    out << (quint16)_world->_score[1];

    //    qDebug() << "bx" << _world->_ballActualPos.x();
    //    qDebug() << "by" << _world->_ballActualPos.y();
    //        qDebug() << "p1" << _world->playerActualPosition(0).x();
    //        qDebug() << "p1" << _world->playerActualPosition(0).y();
    //        qDebug() << "p2" << _world->playerActualPosition(1).x();
    //        qDebug() << "p2" << _world->playerActualPosition(1).y();
    //        qDebug() << "s1" << _world->actualScore(0);
    //        qDebug() << "s2" << _world->actualScore(1);

    for (int i = 0; i < _clients.size(); ++i) {
        _clients[i]->write(packet);
    }
}

void Server::newClient()
{
    QTcpSocket *newClient = _server->nextPendingConnection();
    qDebug() << "Nouveau client : " << newClient->peerAddress().toString();

    // Création du paquet {initialisation}
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)1;
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);
    out << _world->_width;
    out << _world->_height;
    out << _world->_netHeight;
    out << _world->_ballRadius;
    out << _world->_slimeRadius;
    out << _world->_playerSpeed;

    newClient->write(packet);

    _clients << newClient;

    connect(newClient, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(clientOut()));
}

void Server::dataReceived()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

    qint64 packetSize = 2 * sizeof (quint16);
    if (socket->bytesAvailable() < packetSize)
        return;

    QByteArray packet = socket->readAll();
    packet = packet.right(packetSize);

    QDataStream in(&packet, QIODevice::ReadOnly);
    quint16 keys1;
    quint16 keys2;
    in >> keys1;
    in >> keys2;

    qDebug() << "k1 :" << keys1 << " k2 :" << keys2;

    if (_runMutex.tryLock()) {
        if (!(keys1 & 0x8))
            _playersKeys[0] = (World::Movements)keys1;
        if (!(keys2 & 0x8))
            _playersKeys[1] = (World::Movements)keys2;
        double dt = (double)_time.restart() / 1000.0;
        _world->exactMove(dt, _playersKeys);
        _runMutex.unlock();
    } else {
        qDebug("tu envoie trop de paquet ?");
    }
}

void Server::clientOut()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

    qDebug() << "Client deco : " << socket->peerAddress().toString();
    _clients.removeOne(socket);
    socket->deleteLater();
}

void Server::setPort(quint16 port)
{
    _portNumber = port;
}

World * Server::world()
{
    return _world;
}
