#include "server.h"
#include <QTime>
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>

Server::Server(QObject *parent) :
    QThread(parent)
{
    _world = new World();

    _server = new QTcpServer(this);

    _playersKeys[0] = _playersKeys[1] = World::DontMove;
}

Server::~Server()
{
    _stopServer = true;
    wait(1);
    delete _world;
}

void Server::play()
{
    // initialisation
    if (_server->listen(QHostAddress::Any, _portNumber)) {
        qDebug() << "server start to listen on port : " << _portNumber;
        connect(_server, SIGNAL(newConnection()), this, SLOT(newClient()));
    } else {
        qDebug() << "server can't start on port : " << _portNumber;
        QCoreApplication::exit(-1);
        return;
    }

    world()->reset();

    _timerId = startTimer(30);
    // que le jeu commence
    start();
}

void Server::run()
{
    QTime time;
    time.start();

    World::Movements playersKeys[2];

    _stopServer = false;
    while (_stopServer == false) {
        msleep(qMax(15 - time.elapsed(), 4));

        _runMutex.lock();
        double dt = (double)time.restart() / 1000.0;
        for (int i = 0; i < 2; ++i)
            playersKeys[i] = _playersKeys[i];
        _runMutex.unlock();

        // calcule les nouvelles positions
//        QTime t; t.start();

        _world->exactMove(dt, playersKeys);
//        qDebug() << t.elapsed() << "ms";
    }

    // arrêt
    killTimer(_timerId);
    _server->close();
    qDebug("Server stopped");
}

void Server::timerEvent(QTimerEvent *)
{
    // Création du paquet PKT_PLAY
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);
    out << (quint16)0;
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
    out.device()->seek(0);
    out << (quint16)(packet.size() - sizeof (quint16));

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

    // Création du paquet PKT_INIT
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16)0;
    out << _world->_width;
    out << _world->_height;
    out << _world->_netHeight;
    out << _world->_ballRadius;
    out << _world->_slimeRadius;
    out.device()->seek(0);
    quint16 packetSize = packet.size() - sizeof (quint16);
    out << packetSize;
    qDebug() << "packetSize = " << packetSize;

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

    _runMutex.lock();
    _playersKeys[0] = (World::Movements)keys1;
    _playersKeys[1] = (World::Movements)keys2;
    _runMutex.unlock();
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
