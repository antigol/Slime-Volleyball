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
    double width() const;
    double height() const;
    double netHeight() const;
    double playerSpeed() const;
    double gValue() const;
    double enTransFacor() const;
    double ballRadius() const;
    double slimeRadius() const;

    void setwidth(double x);
    void setheight(double x);
    void setnetHeight(double x);
    void setplayerSpeed(double x);
    void setgValue(double x);
    void setEnTransFacor(double x);
    void setBallRadius(double x);
    void setSlimeRadius(double x);

    QPointF ballActualPosition() const;
    QPointF playerActualPosition(int i) const;
    int actualScore(int i) const;

private:
    double length(QPointF p);
    double dotProduct(QPointF p1, QPointF p2);

    // Collision entre deux cercles dans le plan qui ont la même acceleration
    double collisionUniformMotion(double r1x, double r1y, double v1x, double v1y, double R1,
                                     double r2x, double r2y, double v2x, double v2y, double R2);

    QPointF _ballActualPos;
    QPointF _ballSpeedInit;
    QPointF _ballPosInit;
    double _ballTVol;

    QPointF _playersPos[2];
    double _tvol[2];
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
