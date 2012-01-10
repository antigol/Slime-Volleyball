#ifndef WORLD_H
#define WORLD_H

#include <QPointF>

#define WORLD_BALL_RADIUS 15
#define WORLD_SLIME_RADIUS 30

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
    void move(double dt, enum Movements p1, enum Movements p2);
    double playerSpeed() const;

private:
    double length(QPointF p);
    double dotProduct(QPointF p1, QPointF p2);

    QPointF _ballActualPos;
    QPointF _ballSpeedInit;
    QPointF _ballPosInit;
    double _ballTVol;

    QPointF _playersPos[2];
    double _tvol[2];

    // parameters
    double _pSpeed;
    double _gValue;
};

#endif // WORLD_H
