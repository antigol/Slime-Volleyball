#include "server.h"
#include <QTime>
#include <QCoreApplication>
#include <QDataStream>

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

void Server::run()
{
    // initialisation
    if (_server->listen(QHostAddress::Any, _portNumber)) {
        qDebug("serveur démarré sur le port : %d", _portNumber);
        connect(_server, SIGNAL(newConnection()), this, SLOT(newClient()));
    } else {
        qDebug("serveur n'a pas pu démarrer sur le port : %d", _portNumber);
        QCoreApplication::exit(-1);
    }

    QTime time;
    time.start();

    while (_stopServer == false) {
        msleep(25 - time.elapsed());

        _runMutex.lock();
        double dt = (double)time.restart() / 1000.0;

        // calcule les nouvelles positions
        _world->move(dt, World::DontMove, World::DontMove);

        _runMutex.unlock();

        // envoie les nouvelles positions
        // out << world.width();

        // for (... { _clients[i].write(data);
    }

    // arrêt
}

void Server::newClient()
{
    QTcpSocket *newClient = _server->nextPendingConnection();

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16)0;
    out << (float)_world->playerSpeed();
    // et d'autres
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

    if (socket->bytesAvailable() < 2 * sizeof (quint16))
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
}
