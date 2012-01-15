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
    void reset();

private:
    double length(QPointF p);
    double dotProduct(QPointF p1, QPointF p2);

    // Collision entre deux cercles dans le plan
    double collisionBallBall(double r1x, double r1y, double v1x, double v1y, double a1x, double a1y, double R1,
                             double r2x, double r2y, double v2x, double v2y, double a2x, double a2y, double R2);
    // Collision entre un point et un segment vertical (s1y <= s2y)
    double collisionPointVSeg(double r1x, double r1y, double v1x, double v1y, double a1x, double a1y,
                              double sx, double s1y, double s2y);
    double collisionPointHSeg(double r1x, double r1y, double v1x, double v1y, double a1x, double a1y,
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
    int _score[2];


    // parameters
    double _width;
    double _height;
    double _netHeight;
    double _pSpeed;
    double _gValue;
    double _enTransFactor;
    double _ballRadius;
    double _slimeRadius;
};

#endif // WORLD_H
