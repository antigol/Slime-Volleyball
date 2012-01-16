#ifndef SERVER_H
#define SERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMutex>
#include <QTime>
#include "world.h"

class Server : public QThread
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

    void setPort(quint16 port);
    World *world();
    bool play();
    void stop();

private slots:
    void newClient();
    void dataReceived();
    void clientOut();

private:
    void run();
    void timerEvent(QTimerEvent *);

    QTime _time;
    World *_world;
    QMutex _runMutex;
    QTcpServer *_server;
    QList<QTcpSocket *> _clients;
    bool _stopServer;
    World::Movements _playersKeys[2];

    quint16 _portNumber;
    int _timerId;
};

#endif // SERVER_H
