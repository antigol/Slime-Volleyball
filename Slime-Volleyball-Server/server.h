#ifndef SERVER_H
#define SERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMutex>
#include "world.h"

class Server : public QThread
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

    void play();
    void setPort(quint16 port);
    World *world();

private slots:
    void newClient();
    void dataReceived();
    void clientOut();

private:
    void run();

    World *_world;
    QMutex _runMutex;
    QTcpServer *_server;
    QList<QTcpSocket *> _clients;
    bool _stopServer;
    int _playersKeys[2];

    quint16 _portNumber;
};

#endif // SERVER_H
