#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMutex>
#include <QTime>
#include "world.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

    void setPort(quint16 port);
    World *world();
    bool startServer();
    void stopServer();

    void startGame();
    void stopGame();

private slots:
    void clientIn();
    void clientOut();
    void dataReceived();

private:
    void timerEvent(QTimerEvent *);

    QTime _time;
    World *_world;
    QMutex _runMutex;
    QTcpServer *_server;
    QList<QTcpSocket *> _clients;
    World::Movements _playersKeys[2];

    quint16 _portNumber;
    int _timerId;

    bool _isRunning;
};

#endif // SERVER_H
