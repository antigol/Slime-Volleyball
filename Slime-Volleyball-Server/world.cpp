#include "world.h"
#include <cmath>
#include <QRectF>

World::World()
{
}

/*
  collision pour la balle :
    * le plafond
    * le mur gauche
    * le mur droite
    * le sol
    * le fillet à gauche (VSeg - ballRadius)
    * le fillet à droite (VSeg + ballRadius)
    * le sommet du fillet (balle de rayon nul)
    * le joueur 1
    * le joueur 2

*/

void World::move(double dt, Movements playersMov[])
{
    // déplace les objets de dt
    double dx = dt * _pSpeed;

    for (int i = 0; i < 2; ++i) {
        if ((playersMov[i] & Left) &&
                ((i == 0 && _playersActualPos[i].x() > _slimeRadius)
                 || (i == 1 && _playersActualPos[i].x() > _width/2.0 + _slimeRadius))) {
            _playersActualPos[i].rx() -= dx;
            _playersActualSpeed[i].rx() = -_pSpeed;
        } else if ((playersMov[i] & Right) &&
                   ((i == 0 && _playersActualPos[i].x() < _width/2.0 - _slimeRadius)
                    || (i == 1 && _playersActualPos[i].x() < _width - _slimeRadius))){
            _playersActualPos[i].rx() += dx;
            _playersActualSpeed[i].rx() = _pSpeed;
        } else {
            _playersActualSpeed[i].rx() = 0;
        }
        if ((playersMov[i] & Up) && _tvol[i] < 0) {
            // se prépare à voler
            _tvol[i] = 0;
            _playersActualSpeed[i].ry() = _pSpeed;
        }

        if (_tvol[i] >= 0) {
            _tvol[i] += dt;
            double y = _pSpeed * _tvol[i] - 0.5 * _gValue * _tvol[i] * _tvol[i];
            double vy = _pSpeed - _gValue * _tvol[i];
            if (y <= 0) {
                y = vy = 0;
                _tvol[i] = -1;
            }
            _playersActualPos[i].ry() = y;
            _playersActualSpeed[i].ry() = vy;
        }

    }

    // mouvement de la balle
    _ballTVol += dt;
    // calcul de la position exacte (parabole)
    _ballActualPos.rx() = _ballPosInit.x() + _ballSpeedInit.x() * _ballTVol;
    _ballActualPos.ry() = _ballPosInit.y() + _ballSpeedInit.y() * _ballTVol - 0.5 * _gValue * _ballTVol * _ballTVol;
    _ballActualSpeed.rx() = _ballSpeedInit.x();
    _ballActualSpeed.ry() = _ballSpeedInit.y() - _gValue * _ballTVol;

    // collisions balle <-> joueurs
    for (int i = 0; i < 2; ++i) {
        QPointF u = _ballActualPos - _playersActualPos[i];
        double dist = length(u);
        u /= dist;

        if (dist <= _ballRadius + _slimeRadius) {
            // referentiel : playeri
            QPointF s = _ballActualSpeed - _playersActualSpeed[i] * _enTransFactor;
            double factor = dotProduct(s, u);
            // Le facteur doit être negatif, s'il est positif c'est un bug
            if (factor < 0) {
                // debond sans perte d'energie
                s -= 2.0 * factor * u;
                // referentiel : world
                _ballSpeedInit = s + _playersActualSpeed[i] * _enTransFactor;
                _ballPosInit = _ballActualPos;
                _ballTVol = 0;
            }
        }
    }

    // collisions balle <-> bords
    if (_ballActualPos.x() <= _ballRadius && _ballActualSpeed.x() < 0) {
        // modifie la valeur de la vitesse initiale pour v0
        _ballSpeedInit.rx() = -_ballActualSpeed.x();
        _ballSpeedInit.ry() = _ballActualSpeed.y();
        _ballSpeedInit *= 0.95; // les murs sont mous
        // modifie la position initiale de la balle pour le calcul de barabole
        _ballPosInit = _ballActualPos;
        // réinitialise le temps de vol pour la formule : v0*t-1/2g*t²
        _ballTVol = 0;
    }
    if (_ballActualPos.x() >= _width - _ballRadius && _ballActualSpeed.x() > 0) {
        _ballSpeedInit.rx() = -_ballActualSpeed.x();
        _ballSpeedInit.ry() = _ballActualSpeed.y();
        _ballPosInit = _ballActualPos;
        _ballSpeedInit *= 0.95;
        _ballTVol = 0;
    }

    // collision au plafond
    if (_ballActualPos.y() >= _height - _ballRadius && _ballActualSpeed.y() > 0) {
        _ballSpeedInit.rx() = _ballActualSpeed.x();
        _ballSpeedInit.ry() = -_ballActualSpeed.y();
        _ballSpeedInit *= 0.95;
        _ballPosInit = _ballActualPos;
        _ballTVol = 0;
    }

    // collisions avec le filet
    if (QRectF(_width/2.0-_ballRadius, 0, _ballRadius, _netHeight).contains(_ballActualPos)
            && _ballActualSpeed.x() > 0) {
        _ballSpeedInit.rx() = -_ballActualSpeed.x();
        _ballSpeedInit.ry() = _ballActualSpeed.y();
        _ballPosInit = _ballActualPos;
        _ballTVol = 0;
    } else if (QRectF(_width/2.0, 0, _ballRadius, _netHeight).contains(_ballActualPos)
               && _ballActualSpeed.x() < 0) {
        _ballSpeedInit.rx() = -_ballActualSpeed.x();
        _ballSpeedInit.ry() = _ballActualSpeed.y();
        _ballPosInit = _ballActualPos;
        _ballTVol = 0;
    } else {
        // vecteur unitaire u (du sommet du filet -> centre de la balle)
        QPointF u = _ballActualPos - QPointF(_width/2.0, _netHeight);
        double dist = length(u);
        u /= dist;

        if (dist <= _ballRadius) {
            QPointF s = _ballActualSpeed;
            double factor = dotProduct(s, u);
            // Le facteur doit être negatif, s'il est positif c'est que la balle est déjà entrain de repartir
            if (factor < 0) {
                // debond sans perte d'energie
                s -= 2.0 * factor * u;
                // referentiel : world
                _ballSpeedInit = s;
                _ballPosInit = _ballActualPos;
                _ballTVol = 0;
            }
        }
    }

    // collision de la balle avec le sol
    if (_ballActualPos.y() <= _ballRadius) {

        if (_ballActualPos.x() < _width/2.0) {
            _score[1]++;
            _ballPosInit = QPointF(3.0*_width/4.0, _height/2.0);
        } else {
            _score[0]++;
            _ballPosInit = QPointF(_width/4.0, _height/2.0);
        }
        _ballSpeedInit = QPointF(0, 0);
        _ballTVol = 0;

        // on remet les joueurs en place
        _playersActualPos[0] = QPointF(_width/4.0, 0);
        _playersActualPos[1] = QPointF(3.0*_width/4.0, 0);
        _tvol[0] = _tvol[1] = -1;
    }

}

void World::reset()
{
    _playersActualPos[0] = QPointF(_width/4.0, 0);
    _playersActualPos[1] = QPointF(3.0*_width/4.0, 0);
    _tvol[0] = _tvol[1] = -1;
    _ballSpeedInit = QPointF(0, 0);
    _ballTVol = 0;
    _ballPosInit = QPointF(_width/4.0, _height/2.0);
    _score[0] = _score[1] = 0;
}

double World::length(QPointF p)
{
    return std::sqrt(p.x()*p.x() + p.y()*p.y());
}

double World::dotProduct(QPointF p1, QPointF p2)
{
    return p1.x()*p2.x() + p1.y()*p2.y();
}

#include "quartic/quartic.hpp"

double World::collisionBallBall(double r1x, double r1y, double v1x, double v1y, double a1x, double a1y, double R1,
                                double r2x, double r2y, double v2x, double v2y, double a2x, double a2y, double R2)
{
    double drx = r2x - r1x;
    double dry = r2y - r1y;
    double dvx = v2x - v1x;
    double dvy = v2y - v1y;
    double dax = a2x - a1x;
    double day = a2y - a1y;
    double R = R1 + R2;

    double e = (dax*dax + day*day) / 4.0;
    double a = dax*dvx + day*dvy;
    double b = dax*drx + day*dry + dvx*dvx + dvy*dvy;
    double c = 2.0 * (dvx*drx + dvy*dry);
    double d = drx*drx + dry*dry - R*R;

    double root[4] = {-1.0};

    // (e = 0) -> (a1 = a2) -> (a = 0)
    if (e == 0.0) {
        // b t² + c t + d = 0
        if (!magnet::math::quadSolve(d, c, b, root[0], root[1]))
            return -1.0;
    } else {
        a /= e;
        b /= e;
        c /= e;
        d /= e;

        magnet::math::quarticSolve(a, b, c, d, root[0], root[1], root[2], root[3]);
    }

    // la plus petite racine positive

    double r = -1.0;
    for (int i = 0; i < 4; ++i) {
        if (root[i] >= 0.0 && root[i] < r)
            r = root[i];
    }

    return r;
}

double World::collisionPointVSeg(double r1x, double r1y, double v1x, double v1y, double a1x, double a1y, double sx, double s1y, double s2y)
{
    double a = a1x / 2.0;
    double b = v1x;
    double c = r1x - sx;

    double t1, t2;

    if (!magnet::math::quadSolve(c, b, a, t1, t2))
        return -1.0;

    double y1 = r1y + v1y * t1 + 0.5 * a1y * t1*t1;
    double y2 = r1y + v1y * t2 + 0.5 * a1y * t2*t2;

    // dehors du segment
    if (y1 < s1y || y1 > s2y) {
        t1 = -1.0;
    }
    if (y2 < s1y || y2 > s2y) {
        t2 = -1.0;
    }

    if (t1 > 0.0 && (t1 < t2 || t2 < 0.0))
        return t1;
    return t2;
}

double World::collisionPointHSeg(double r1x, double r1y, double v1x, double v1y, double a1x, double a1y, double sy, double s1x, double s2x)
{
    double a = a1y / 2.0;
    double b = v1y;
    double c = r1y - sy;

    double t1, t2;

    if (!magnet::math::quadSolve(c, b, a, t1, t2))
        return -1.0;

    double x1 = r1x + v1x * t1 + 0.5 * a1x * t1*t1;
    double x2 = r1x + v1x * t2 + 0.5 * a1x * t2*t2;

    // dehors du segment
    if (x1 < s1x || x1 > s2x) {
        t1 = -1.0;
    }
    if (x2 < s1x || x2 > s2x) {
        t2 = -1.0;
    }

    if (t1 > 0.0 && (t1 < t2 || t2 < 0.0))
        return t1;
    return t2;
}
