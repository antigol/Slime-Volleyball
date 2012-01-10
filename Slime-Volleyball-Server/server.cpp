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
        qDebug() << QString::fromUtf8("serveur demarre sur le port : ") << _portNumber;
        connect(_server, SIGNAL(newConnection()), this, SLOT(newClient()));
    } else {
        qDebug() << "serveur n'a pas pu demarrer sur le port : " << _portNumber;
        QCoreApplication::exit(-1);
    }

    world()->reset();
    start();
}

void Server::run()
{
    QTime time;
    time.start();

    while (_stopServer == false) {
        msleep(25 - time.elapsed());

        _runMutex.lock();
        double dt = (double)time.restart() / 1000.0;

        // calcule les nouvelles positions
        _world->move(dt, World::DontMove, World::DontMove);

        _runMutex.unlock();

        // Création du paquet PKT_PLAY
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint16)0;
        out << (float)_world->ballActualPosition().x();
        out << (float)_world->ballActualPosition().y();
        out << (float)_world->playerActualPosition(0).x();
        out << (float)_world->playerActualPosition(0).y();
        out << (float)_world->playerActualPosition(1).x();
        out << (float)_world->playerActualPosition(1).y();
        out << (float)_world->actualScore(0);
        out << (float)_world->actualScore(1);

        qDebug() << "bx" << (float)_world->ballActualPosition().x();
        qDebug() << "by" << (float)_world->ballActualPosition().y();
        qDebug() << "p1" << (float)_world->playerActualPosition(0).x();
        qDebug() << "p1" << (float)_world->playerActualPosition(0).y();
        qDebug() << "p2" << (float)_world->playerActualPosition(1).x();
        qDebug() << "p2" << (float)_world->playerActualPosition(1).y();
        qDebug() << "s1" << (float)_world->actualScore(0);
        qDebug() << "s2" << (float)_world->actualScore(1);
        out.device()->seek(0);
        out << (quint16)(packet.size() - sizeof (quint16));

        for (int i = 0; i < _clients.size(); ++i) {
            _clients[i]->write(packet);
        }
    }

    // arrêt
    _server->close();
}

void Server::newClient()
{
    QTcpSocket *newClient = _server->nextPendingConnection();
    qDebug() << "Nouveau client : " << newClient->peerAddress().toString();

    // Création du paquet PKT_INIT
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16)0;
    out << (float)_world->width();
    out << (float)_world->height();
    out << (float)_world->netHeight();
    out.device()->seek(0);
    out << (quint16)(packet.size() - sizeof (quint16));

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

    if (socket->bytesAvailable() < (qint64)(2 * sizeof (quint16)))
        return;

    QDataStream in(socket);
    quint16 playerNumber;
    quint16 keys;
    in >> playerNumber;
    in >> keys;

    if (playerNumber < 2) {
        _playersKeys[playerNumber] = keys;
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
