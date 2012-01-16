#include "world.h"
#include <cmath>
#include <QRectF>
#include <QDebug>

World::World()
{
}

void World::reset(int team)
{
    _playersActualPos[0] = QPointF(_width/4.0, 0);
    _playersActualPos[1] = QPointF(3.0*_width/4.0, 0);
    _playersActualSpeed[0] =
            _playersActualSpeed[1] =
            _playersActualAccel[0] =
            _playersActualAccel[1] = QPointF(0, 0);

    if (team == 0)
        _ballActualPos = QPointF(_width/4.0, _height/2.0);
    else
        _ballActualPos = QPointF(3.0*_width/4.0, _height/2.0);
    _ballActualSpeed = QPointF(0, 0);

    if (team == -1)
        _score[0] = _score[1] = 0;
}

//void World::move(double dt, Movements playersMov[])
//{
//    // déplace les objets de dt
//    double dx = dt * _playerSpeed;

//    for (int i = 0; i < 2; ++i) {
//        if ((playersMov[i] & Left) &&
//                ((i == 0 && _playersActualPos[i].x() > _slimeRadius)
//                 || (i == 1 && _playersActualPos[i].x() > _width/2.0 + _slimeRadius))) {
//            _playersActualPos[i].rx() -= dx;
//            _playersActualSpeed[i].rx() = -_playerSpeed;
//        } else if ((playersMov[i] & Right) &&
//                   ((i == 0 && _playersActualPos[i].x() < _width/2.0 - _slimeRadius)
//                    || (i == 1 && _playersActualPos[i].x() < _width - _slimeRadius))){
//            _playersActualPos[i].rx() += dx;
//            _playersActualSpeed[i].rx() = _playerSpeed;
//        } else {
//            _playersActualSpeed[i].rx() = 0;
//        }
//        if ((playersMov[i] & Up) && _tvol[i] < 0) {
//            // se prépare à voler
//            _tvol[i] = 0;
//            _playersActualSpeed[i].ry() = _playerSpeed;
//        }

//        if (_tvol[i] >= 0) {
//            _tvol[i] += dt;
//            double y = _playerSpeed * _tvol[i] - 0.5 * _gValue * _tvol[i] * _tvol[i];
//            double vy = _playerSpeed - _gValue * _tvol[i];
//            if (y <= 0) {
//                y = vy = 0;
//                _tvol[i] = -1;
//            }
//            _playersActualPos[i].ry() = y;
//            _playersActualSpeed[i].ry() = vy;
//        }

//    }

//    // mouvement de la balle
//    _ballTVol += dt;
//    // calcul de la position exacte (parabole)
//    _ballActualPos.rx() = _ballPosInit.x() + _ballSpeedInit.x() * _ballTVol;
//    _ballActualPos.ry() = _ballPosInit.y() + _ballSpeedInit.y() * _ballTVol - 0.5 * _gValue * _ballTVol * _ballTVol;
//    _ballActualSpeed.rx() = _ballSpeedInit.x();
//    _ballActualSpeed.ry() = _ballSpeedInit.y() - _gValue * _ballTVol;

//    // collisions balle <-> joueurs
//    for (int i = 0; i < 2; ++i) {
//        QPointF u = _ballActualPos - _playersActualPos[i];
//        double dist = std::sqrt(sqlength(u));
//        u /= dist;

//        if (dist <= _ballRadius + _slimeRadius) {
//            // referentiel : playeri
//            QPointF s = _ballActualSpeed - _playersActualSpeed[i] * _enTransFactor;
//            double factor = dotProduct(s, u);
//            // Le facteur doit être negatif, s'il est positif c'est un bug
//            if (factor < 0) {
//                // debond sans perte d'energie
//                s -= 2.0 * factor * u;
//                // referentiel : world
//                _ballSpeedInit = s + _playersActualSpeed[i] * _enTransFactor;
//                _ballPosInit = _ballActualPos;
//                _ballTVol = 0;
//            }
//        }
//    }

//    // collisions balle <-> bords
//    if (_ballActualPos.x() <= _ballRadius && _ballActualSpeed.x() < 0) {
//        // modifie la valeur de la vitesse initiale pour v0
//        _ballSpeedInit.rx() = -_ballActualSpeed.x();
//        _ballSpeedInit.ry() = _ballActualSpeed.y();
//        _ballSpeedInit *= 0.95; // les murs sont mous
//        // modifie la position initiale de la balle pour le calcul de barabole
//        _ballPosInit = _ballActualPos;
//        // réinitialise le temps de vol pour la formule : v0*t-1/2g*t²
//        _ballTVol = 0;
//    }
//    if (_ballActualPos.x() >= _width - _ballRadius && _ballActualSpeed.x() > 0) {
//        _ballSpeedInit.rx() = -_ballActualSpeed.x();
//        _ballSpeedInit.ry() = _ballActualSpeed.y();
//        _ballPosInit = _ballActualPos;
//        _ballSpeedInit *= 0.95;
//        _ballTVol = 0;
//    }

//    // collision au plafond
//    if (_ballActualPos.y() >= _height - _ballRadius && _ballActualSpeed.y() > 0) {
//        _ballSpeedInit.rx() = _ballActualSpeed.x();
//        _ballSpeedInit.ry() = -_ballActualSpeed.y();
//        _ballSpeedInit *= 0.95;
//        _ballPosInit = _ballActualPos;
//        _ballTVol = 0;
//    }

//    // collisions avec le filet
//    if (QRectF(_width/2.0-_ballRadius, 0, _ballRadius, _netHeight).contains(_ballActualPos)
//            && _ballActualSpeed.x() > 0) {
//        _ballSpeedInit.rx() = -_ballActualSpeed.x();
//        _ballSpeedInit.ry() = _ballActualSpeed.y();
//        _ballPosInit = _ballActualPos;
//        _ballTVol = 0;
//    } else if (QRectF(_width/2.0, 0, _ballRadius, _netHeight).contains(_ballActualPos)
//               && _ballActualSpeed.x() < 0) {
//        _ballSpeedInit.rx() = -_ballActualSpeed.x();
//        _ballSpeedInit.ry() = _ballActualSpeed.y();
//        _ballPosInit = _ballActualPos;
//        _ballTVol = 0;
//    } else {
//        // vecteur unitaire u (du sommet du filet -> centre de la balle)
//        QPointF u = _ballActualPos - QPointF(_width/2.0, _netHeight);
//        double dist = std::sqrt(sqlength(u));
//        u /= dist;

//        if (dist <= _ballRadius) {
//            QPointF s = _ballActualSpeed;
//            double factor = dotProduct(s, u);
//            // Le facteur doit être negatif, s'il est positif c'est que la balle est déjà entrain de repartir
//            if (factor < 0) {
//                // debond sans perte d'energie
//                s -= 2.0 * factor * u;
//                // referentiel : world
//                _ballSpeedInit = s;
//                _ballPosInit = _ballActualPos;
//                _ballTVol = 0;
//            }
//        }
//    }

//    // collision de la balle avec le sol
//    if (_ballActualPos.y() <= _ballRadius) {

//        if (_ballActualPos.x() < _width/2.0) {
//            _score[1]++;
//            _ballPosInit = QPointF(3.0*_width/4.0, _height/2.0);
//        } else {
//            _score[0]++;
//            _ballPosInit = QPointF(_width/4.0, _height/2.0);
//        }
//        _ballSpeedInit = QPointF(0, 0);
//        _ballTVol = 0;

//        // on remet les joueurs en place
//        _playersActualPos[0] = QPointF(_width/4.0, 0);
//        _playersActualPos[1] = QPointF(3.0*_width/4.0, 0);
//        _tvol[0] = _tvol[1] = -1;
//    }

//}

double World::sqlength(QPointF p)
{
    return (p.x()*p.x() + p.y()*p.y());
}

double World::dotProduct(QPointF p1, QPointF p2)
{
    return p1.x()*p2.x() + p1.y()*p2.y();
}

/*
  collision pour la balle :
    0* le joueur 1
    1* le joueur 2
    2* le plafond - r
    3* le mur gauche + r
    4* le mur droite - r
    5* le sol + r
    6* le fillet à gauche (VSeg - ballRadius)
    7* le fillet à droite (VSeg + ballRadius)
    8* le sommet du fillet (balle de rayon nul)

  collision pour le joueur 1 :
    9* le mur gauche + R
   10* le fillet - R
   11* le sol
   12* le plafond - R

*/

void World::exactMove(double dt, World::Movements playersMov[])
{
    QPointF ballActualAccel(0, -_gValue);

    // On ne déplace pas les objets ! On change leur vitesse et acceleration

    if ((playersMov[0] & Left) && _playersActualPos[0].x() != _slimeRadius) {
        _playersActualSpeed[0].rx() = -_playerSpeed;
    } else if ((playersMov[0] & Right) && _playersActualPos[0].x() != _width/2.0 - _slimeRadius) {
        _playersActualSpeed[0].rx() = _playerSpeed;
    } else {
        _playersActualSpeed[0].rx() = 0;
    }

    if ((playersMov[1] & Left) && _playersActualPos[1].x() != _width/2.0 + _slimeRadius) {
        _playersActualSpeed[1].rx() = -_playerSpeed;
    } else if ((playersMov[1] & Right) && _playersActualPos[1].x() != _width - _slimeRadius) {
        _playersActualSpeed[1].rx() = _playerSpeed;
    } else {
        _playersActualSpeed[1].rx() = 0;
    }

    for (int i = 0; i < 2; ++i) {
        if ((playersMov[i] & Up) && _playersActualAccel[i].y() == 0.0) {
            // se prépare à voler
            _playersActualSpeed[i].ry() = _playerSpeed;
            _playersActualAccel[i].ry() = -_gValue;
        }
    }

    // ===== Calcul des temps avant impacte =====
    double tt = -1;
    int index = -1;

    double tmp;
    // 9 + 4 + 4 = 17 collisions à calculer !!

    // 0-1 : balle - joueur 1-2
    for (int i = 0; i < 2; ++i) {
        tmp = collisionBallBall(_ballActualPos, _ballActualSpeed,
                                ballActualAccel, _ballRadius,
                                _playersActualPos[i], _playersActualSpeed[i],
                                _playersActualAccel[i], _slimeRadius);

        if (tmp >= 0.0 && (index < 0 || tmp < tt)) {
            index = i;
            tt = tmp;
        }
    }
    // 2 : le plafond - r
    tmp = collisionPointHSeg(_ballActualPos, _ballActualSpeed, ballActualAccel, _height - _ballRadius, 0, _width);
    if (_ballActualSpeed.y() > 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 2;
        tt = tmp;
    }
    // 3 : mur gauche
    tmp = collisionPointVSeg(_ballActualPos, _ballActualSpeed, ballActualAccel, _ballRadius, 0, _height);
    if (_ballActualSpeed.x() < 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 3;
        tt = tmp;
    }
    // 4 : mur  droite
    tmp = collisionPointVSeg(_ballActualPos, _ballActualSpeed, ballActualAccel, _width - _ballRadius, 0, _height);
    if (_ballActualSpeed.x() > 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 4;
        tt = tmp;
    }
    // 5 : le sol
    tmp = collisionPointHSeg(_ballActualPos, _ballActualSpeed, ballActualAccel, _ballRadius, 0, _width);
    if (_ballActualSpeed.y() < 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 5;
        tt = tmp;
    }
    // 6 : fillet à gauche
    tmp = collisionPointVSeg(_ballActualPos, _ballActualSpeed, ballActualAccel, _width/2.0 - _ballRadius, 0, _netHeight);
    if (_ballActualSpeed.x() > 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 6;
        tt = tmp;
    }
    // 7 : fillet à droite
    tmp = collisionPointVSeg(_ballActualPos, _ballActualSpeed, ballActualAccel, _width/2.0 + _ballRadius, 0, _netHeight);
    if (_ballActualSpeed.x() < 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 7;
        tt = tmp;
    }
    // 8 : fillet somet
    tmp = collisionBallBall(_ballActualPos, _ballActualSpeed, ballActualAccel, _ballRadius,
                            QPointF(_width/2.0, _netHeight), QPointF(0, 0), QPointF(0, 0), 0.0);
    if (tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 8;
        tt = tmp;
    }

    // 9 : mur
    tmp = collisionPointVSeg(_playersActualPos[0], _playersActualSpeed[0], _playersActualAccel[0], _slimeRadius, 0, _height);
    if (_playersActualSpeed[0].x() < 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 9;
        tt = tmp;
    }
    // 10 : fillet
    tmp = collisionPointVSeg(_playersActualPos[0], _playersActualSpeed[0], _playersActualAccel[0], _width/2.0 - _slimeRadius, 0, _height);
    if (_playersActualSpeed[0].x() > 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 10;
        tt = tmp;
    }

    for (int i = 0; i < 2; ++i) {
        // 11 : joueur sol + R
        tmp = collisionPointHSeg(_playersActualPos[i], _playersActualSpeed[i], _playersActualAccel[i], 0, 0, _width);
        if (_playersActualSpeed[i].y() < 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
            index = 4*i + 11;
            tt = tmp;
        }
        // 12 : plafond
        tmp = collisionPointHSeg(_playersActualPos[i], _playersActualSpeed[i], _playersActualAccel[i], _height - _slimeRadius, 0, _width);
        if (_playersActualSpeed[i].y() > 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
            index = 4*i + 12;
            tt = tmp;
        }
    }

    // 13 : mur
    tmp = collisionPointVSeg(_playersActualPos[1], _playersActualSpeed[1], _playersActualAccel[1], _width - _slimeRadius, 0, _height);
    if (_playersActualSpeed[1].x() > 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 13;
        tt = tmp;
    }
    // 14 : fillet
    tmp = collisionPointVSeg(_playersActualPos[1], _playersActualSpeed[1], _playersActualAccel[1], _width/2.0 + _slimeRadius, 0, _height);
    if (_playersActualSpeed[1].x() < 0.0 && tmp >= 0.0 && (index < 0 || tmp < tt)) {
        index = 14;
        tt = tmp;
    }

    // On déplace les objets
    if (index < 0 || tt >= dt) {
        // de dt
        _ballActualPos += (_ballActualSpeed + 0.5 * ballActualAccel * dt) * dt;
        _ballActualSpeed += ballActualAccel * dt;

        for (int i = 0; i < 2; ++i) {
            _playersActualPos[i] += (_playersActualSpeed[i] + 0.5 * _playersActualAccel[i] * dt) * dt;
            _playersActualSpeed[i] += _playersActualAccel[i] * dt;
        }

    } else {
        if (tt == 0) {
            qDebug() << "tt = " << tt << " index = " << index;
        }

        // On déplace tout de tt
        _ballActualPos += (_ballActualSpeed + 0.5 * ballActualAccel * tt) * tt;
        _ballActualSpeed += ballActualAccel * tt;

        for (int i = 0; i < 2; ++i) {
            _playersActualPos[i] += (_playersActualSpeed[i] + 0.5 * _playersActualAccel[i] * tt) * tt;
            _playersActualSpeed[i] += _playersActualAccel[i] * tt;
        }

        // ===== Gestion de la collision =====
        if (index < 2) {
            // Collision balle - joueur

            // vecteur u, du joueur vers la balle
            QPointF u = _ballActualPos - _playersActualPos[index];
            double dist = std::sqrt(sqlength(u));
            u /= dist;

            // referentiel : playeri
            // fixme:playerspeed
            QPointF s = _ballActualSpeed - _playersActualSpeed[index];
            double factor = dotProduct(s, u);
            // Le facteur doit être negatif, s'il est positif c'est un bug
            if (factor < 0) {
                // debond sans perte d'energie
                s -= 2.0 * factor * u;
                // referentiel : world
                _ballActualSpeed = s + _playersActualSpeed[index];
                double speed = std::sqrt(sqlength(_ballActualSpeed));
                if (speed > _ballMaximumSpeed) {
                    _ballActualSpeed /= speed;
                    _ballActualSpeed *= _ballMaximumSpeed;
                }
            } else {

            }
        }
        if (index == 2) {
            // plafond
            _ballActualSpeed.ry() *= -1.0;
        }
        if (index == 3 || index == 4) {
            // mur gauche droite
            _ballActualSpeed.rx() *= -1.0;
        }
        if (index == 5) {
            // sol
            if (_ballActualPos.x() < _width / 2.0) {
                _score[1]++;
                reset(1);
            } else {
                _score[0]++;
                reset(0);
            }
        }
        if (index == 6 || index == 7) {
            // fillet gauche, droite
            _ballActualSpeed.rx() *= -1.0;
        }
        if (index == 8) {
            QPointF u = _ballActualPos - QPointF(_width/2.0, _netHeight);
            double dist = std::sqrt(sqlength(u));
            u /= dist;

            QPointF s = _ballActualSpeed;
            double factor = dotProduct(s, u);
            // Le facteur doit être negatif, s'il est positif c'est un bug
            if (factor < 0) {
                // debond sans perte d'energie
                s -= 2.0 * factor * u;
                _ballActualSpeed = s;
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (index == 4*i + 9 || index == 4*i + 10) {
                // mur, fillet
                _playersActualSpeed[i].rx() = 0.0;
                _playersActualAccel[i].rx() = 0.0;
            }
            if (index == 4*i + 11) {
                // sol
                _playersActualSpeed[i].ry() = 0.0;
                _playersActualAccel[i].ry() = 0.0;
            }
            if (index == 4*i + 12) {
                // plafond
                _playersActualSpeed[i].ry() *= -1.0;
            }
        }

        // ===== Recurtion ! =====
        exactMove(dt - tt, playersMov);
    }
}

#include "quartic/quartic.hpp"

double World::collisionBallBall(const QPointF &r1, const QPointF &v1, const QPointF &a1, double R1,
                                const QPointF &r2, const QPointF &v2, const QPointF &a2, double R2)
{
    double drx = r2.x() - r1.x();
    double dry = r2.y() - r1.y();
    double dvx = v2.x() - v1.x();
    double dvy = v2.y() - v1.y();
    double dax = a2.x() - a1.x();
    double day = a2.y() - a1.y();
    double R = R1 + R2;

    double e = (dax*dax + day*day) / 4.0;
    double a = dax*dvx + day*dvy;
    double b = dax*drx + day*dry + dvx*dvx + dvy*dvy;
    double c = 2.0 * (dvx*drx + dvy*dry);
    double d = drx*drx + dry*dry - R*R;

    double root[4];

    // (e = 0) -> (a1 = a2) -> (a = 0)
    if (e == 0.0) {
        // b t² + c t + d = 0

        // [ (b = 0) et (c = 0) ==> false ]
        // <==>   [ (v1 = v2) ==> false ] car [ (b = 0) <==> (v1 = v2) ]
        if (!magnet::math::quadSolve(d, c, b, root[0], root[1]))
            return -1.0;
        // retourne -1 si (a1 = a2) et (v1 = v2)

        root[2] = root[3] = -1.0;
    } else {
        int nr = magnet::math::quarticSolve(a/e, b/e, c/e, d/e, root[0], root[1], root[2], root[3]);

        for (int i = nr; i < 4; ++i) root[i] = -1.0;
    }

    // verification de l'accroisement
    for (int i = 0; i < 2; ++i) {
        if (root[i] >= 0.0) {
            double deviation = ((4.0 * e * root[i] + 3.0 * a) * root[i] + 2.0 * b) * root[i] + c;
            if (deviation >= 0.0) {
//                qDebug() << "deviation=" << deviation;
                root[i] = -1.0;
            }
        }
    }

    // la plus petite racine positive

    double r = -1.0;
    for (int i = 0; i < 4; ++i) {
        if (root[i] >= 0.0 && (root[i] < r || r < 0.0))
            r = root[i];
    }

    if (r == 0.0) {
        double deviation = ((4.0 * e * r + 3.0 * a) * r + 2.0 * b) * r + c;
        qDebug() << "t=0 : deviation=" << deviation;
    }

    return r;
}

double World::collisionPointVSeg(const QPointF &r1, const QPointF &v1, const QPointF &a1, double sx, double s1y, double s2y)
{
    double a = a1.x() / 2.0;
    double b = v1.x();
    double c = r1.x() - sx;

    double t1, t2;

    if (!magnet::math::quadSolve(c, b, a, t1, t2))
        return -1.0;

    double y1 = r1.y() + (v1.y() + 0.5 * a1.y() * t1) * t1;
    double y2 = r1.y() + (v1.y() + 0.5 * a1.y() * t2) * t2;

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

double World::collisionPointHSeg(const QPointF &r1, const QPointF &v1, const QPointF &a1, double sy, double s1x, double s2x)
{
    double a = a1.y() / 2.0;
    double b = v1.y();
    double c = r1.y() - sy;

    double t1, t2;

    // si l'acceleration et la vitesse vertiale sont nulles sa retourne -1
    if (!magnet::math::quadSolve(c, b, a, t1, t2))
        return -1.0;

    double x1 = r1.x() + (v1.x() + 0.5 * a1.x() * t1) * t1;
    double x2 = r1.x() + (v1.x() + 0.5 * a1.x() * t2) * t2;

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

