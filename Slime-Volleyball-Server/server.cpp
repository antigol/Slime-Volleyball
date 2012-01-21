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
    stopServer();
    delete _world;
}

bool Server::startServer()
{
    stopServer();

    // démarre le serveur
    if (_server->listen(QHostAddress::Any, _portNumber)) {
        qDebug() << "Server started to listen on port : " << _portNumber;
        connect(_server, SIGNAL(newConnection()), this, SLOT(clientIn()));
    } else {
        qDebug() << "Server can't start on port : " << _portNumber;
        return false;
    }

    world()->reset();

    return true;
}

void Server::stopServer()
{
    // arrêt du jeu
    stopGame();
    // arrêt du serveur
    _server->close();
    _clients.clear();

    qDebug("Server stopped");
}

void Server::startGame()
{
    _isRunning = true;

    _time.start();

    if (_timerId == 0)
        _timerId = startTimer(50);

    qDebug("Game started");
}

void Server::stopGame()
{
    _isRunning = false;

    if (_timerId != 0)
        killTimer(_timerId);
    _timerId = 0;

    qDebug("Game stopped");
}

void Server::clientIn()
{
    QTcpSocket *newClient = _server->nextPendingConnection();

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

    if (_clients.size() == 1)
        startGame();

    qDebug() << "ClientIn : " << newClient->peerAddress().toString();
}

void Server::clientOut()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

    _clients.removeOne(socket);

    socket->deleteLater();

    if (_clients.size() == 0)
        stopGame();

    qDebug() << "ClientOut : " << socket->peerAddress().toString();
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

//    qDebug() << "keys 1 :" << keys1 << " keys 2 :" << keys2;

    if (_runMutex.tryLock()) {
        if (!(keys1 & 0x8))
            _playersKeys[0] = (World::Movements)keys1;
        if (!(keys2 & 0x8))
            _playersKeys[1] = (World::Movements)keys2;
        double dt = (double)_time.restart() / 1000.0;
        _world->exactMove(dt, _playersKeys);
        _runMutex.unlock();
    } else {
        qDebug("Cannot lock run mutex !");
    }
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

    for (int i = 0; i < _clients.size(); ++i) {
        _clients[i]->write(packet);
    }
}

void Server::setPort(quint16 port)
{
    _portNumber = port;
}

World * Server::world()
{
    return _world;
}
