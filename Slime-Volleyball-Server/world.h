#ifndef WORLD_H
#define WORLD_H

#include <QPointF>

class World
{
public:
    enum Movements {
        DontMove = 0x0,
        Left = 0x1,
        Right = 0x2,
        Up = 0x4
    };

    World();
    void move(double dt, enum Movements playersKeys[]);
    void exactMove(double dt, enum Movements playersKeys[]);
    void reset(int team = -1);

private:
    double sqlength(QPointF p);
    double dotProduct(QPointF p1, QPointF p2);

    // Collision entre deux cercles dans le plan
    double collisionBallBall(const QPointF &r1, const QPointF &v1, const QPointF& a1, double R1,
                             const QPointF &r2, const QPointF &v2, const QPointF& a2, double R2);
    // Collision entre un point et un segment vertical (s1y <= s2y)
    double collisionPointVSeg(const QPointF &r1, const QPointF &v1, const QPointF& a1,
                              double sx, double s1y, double s2y);
    double collisionPointHSeg(const QPointF &r1, const QPointF &v1, const QPointF& a1,
                              double sy, double s1x, double s2x);

    QPointF _ballSpeedInit;
    QPointF _ballPosInit;
    double _ballTVol;

    double _tvol[2];

public:
    QPointF _ballActualPos;
    QPointF _ballActualSpeed;
    QPointF _playersActualPos[2];
    QPointF _playersActualSpeed[2];
    QPointF _playersActualAccel[2];
    int _score[2];


    // parameters
    double _width;
    double _height;
    double _netHeight;
    double _playerSpeed;
    double _gValue;
    double _enTransFactor;
    double _ballRadius;
    double _slimeRadius;
    double _ballMaximumSpeed;
};

#endif // WORLD_H
