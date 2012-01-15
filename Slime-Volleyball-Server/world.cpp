#include "world.h"
#include <cmath>
#include <QRectF>

#include "quartic/quartic.hpp"

World::World()
{
}

void World::move(double dt, Movements playersMov[])
{
    // déplace les objets de dt
    double dx = dt * _pSpeed;

    // mouvements des joueurs
    QPointF playersSpeed[2];

    for (int i = 0; i < 2; ++i) {
        if ((playersMov[i] & Left) &&
                ((i == 0 && _playersPos[i].x() > _slimeRadius)
                 || (i == 1 && _playersPos[i].x() > _width/2.0 + _slimeRadius))) {
            _playersPos[i].rx() -= dx;
            playersSpeed[i].rx() = -_pSpeed;
        } else if ((playersMov[i] & Right) &&
                   ((i == 0 && _playersPos[i].x() < _width/2.0 - _slimeRadius)
                    || (i == 1 && _playersPos[i].x() < _width - _slimeRadius))){
            _playersPos[i].rx() += dx;
            playersSpeed[i].rx() = _pSpeed;
        } else {
            playersSpeed[i].rx() = 0;
        }
        if ((playersMov[i] & Up) && _tvol[i] < 0) {
            // se prépare à voler
            _tvol[i] = 0;
        }

        if (_tvol[i] >= 0) {
            _tvol[i] += dt;
            double y = _pSpeed * _tvol[i] - 0.5 * _gValue * _tvol[i] * _tvol[i];
            double vy = _pSpeed - _gValue * _tvol[i];
            if (y <= 0) {
                y = vy = 0;
                _tvol[i] = -1;
            }
            _playersPos[i].ry() = y;
            playersSpeed[i].ry() = vy;
        }

    }

    // mouvement de la balle
    _ballTVol += dt;
    // calcul de la position exacte (parabole)
    _ballActualPos.rx() = _ballPosInit.x() + _ballSpeedInit.x() * _ballTVol;
    _ballActualPos.ry() = _ballPosInit.y() + _ballSpeedInit.y() * _ballTVol - 0.5 * _gValue * _ballTVol * _ballTVol;
    QPointF ballActualSpeed;
    ballActualSpeed.rx() = _ballSpeedInit.x();
    ballActualSpeed.ry() = _ballSpeedInit.y() - _gValue * _ballTVol;

    // collisions balle <-> joueurs
    for (int i = 0; i < 2; ++i) {
        QPointF u = _ballActualPos - _playersPos[i];
        double dist = length(u);
        u /= dist;

        if (dist <= _ballRadius + _slimeRadius) {
            // referentiel : playeri
            QPointF s = ballActualSpeed - playersSpeed[i] * _enTransFactor;
            double factor = dotProduct(s, u);
            // Le facteur doit être negatif, s'il est positif c'est un bug
            if (factor < 0) {
                // debond sans perte d'energie
                s -= 2.0 * factor * u;
                // referentiel : world
                _ballSpeedInit = s + playersSpeed[i] * _enTransFactor;
                _ballPosInit = _ballActualPos;
                _ballTVol = 0;
            }
        }
    }

    // collisions balle <-> bords
    if (_ballActualPos.x() <= _ballRadius && ballActualSpeed.x() < 0) {
        // modifie la valeur de la vitesse initiale pour v0
        _ballSpeedInit.rx() = -ballActualSpeed.x();
        _ballSpeedInit.ry() = ballActualSpeed.y();
        _ballSpeedInit *= 0.95; // les murs sont mous
        // modifie la position initiale de la balle pour le calcul de barabole
        _ballPosInit = _ballActualPos;
        // réinitialise le temps de vol pour la formule : v0*t-1/2g*t²
        _ballTVol = 0;
    }
    if (_ballActualPos.x() >= _width - _ballRadius && ballActualSpeed.x() > 0) {
        _ballSpeedInit.rx() = -ballActualSpeed.x();
        _ballSpeedInit.ry() = ballActualSpeed.y();
        _ballPosInit = _ballActualPos;
        _ballSpeedInit *= 0.95;
        _ballTVol = 0;
    }

    // collision au plafond
    if (_ballActualPos.y() >= _height - _ballRadius && ballActualSpeed.y() > 0) {
        _ballSpeedInit.rx() = ballActualSpeed.x();
        _ballSpeedInit.ry() = -ballActualSpeed.y();
        _ballSpeedInit *= 0.95;
        _ballPosInit = _ballActualPos;
        _ballTVol = 0;
    }

    // collisions avec le filet
    if (QRectF(_width/2.0-_ballRadius, 0, _ballRadius, _netHeight).contains(_ballActualPos)
            && ballActualSpeed.x() > 0) {
        _ballSpeedInit.rx() = -ballActualSpeed.x();
        _ballSpeedInit.ry() = ballActualSpeed.y();
        _ballPosInit = _ballActualPos;
        _ballTVol = 0;
    } else if (QRectF(_width/2.0, 0, _ballRadius, _netHeight).contains(_ballActualPos)
               && ballActualSpeed.x() < 0) {
        _ballSpeedInit.rx() = -ballActualSpeed.x();
        _ballSpeedInit.ry() = ballActualSpeed.y();
        _ballPosInit = _ballActualPos;
        _ballTVol = 0;
    } else {
        // vecteur unitaire u (du sommet du filet -> centre de la balle)
        QPointF u = _ballActualPos - QPointF(_width/2.0, _netHeight);
        double dist = length(u);
        u /= dist;

        if (dist <= _ballRadius) {
            QPointF s = ballActualSpeed;
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
        _playersPos[0] = QPointF(_width/4.0, 0);
        _playersPos[1] = QPointF(3.0*_width/4.0, 0);
        _tvol[0] = _tvol[1] = -1;
    }

}

void World::reset()
{
    _playersPos[0] = QPointF(_width/4.0, 0);
    _playersPos[1] = QPointF(3.0*_width/4.0, 0);
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

double World::width() const
{
    return _width;
}

double World::height() const
{
    return _height;
}

double World::netHeight() const
{
    return _netHeight;
}

double World::playerSpeed() const
{
    return _pSpeed;
}

double World::gValue() const
{
    return _gValue;
}

QPointF World::ballActualPosition() const
{
    return _ballActualPos;
}

QPointF World::playerActualPosition(int i) const
{
    return _playersPos[i];
}

int World::actualScore(int i) const
{
    return _score[i];
}

void World::setwidth(double x)
{
    _width = x;
}

void World::setheight(double x)
{
    _height = x;
}

void World::setnetHeight(double x)
{
    _netHeight = x;
}

void World::setplayerSpeed(double x)
{
    _pSpeed = x;
}

void World::setgValue(double x)
{
    _gValue = x;
}

double World::enTransFacor() const
{
    return _enTransFactor;
}

double World::ballRadius() const
{
    return _ballRadius;
}

double World::slimeRadius() const
{
    return _slimeRadius;
}

void World::setEnTransFacor(double x)
{
    _enTransFactor = x;
}

void World::setBallRadius(double x)
{
    _ballRadius = x;
}

void World::setSlimeRadius(double x)
{
    _slimeRadius = x;
}

double World::collisionUniformMotion(double r1x, double r1y, double v1x, double v1y, double R1, double r2x, double r2y, double v2x, double v2y, double R2)
{
    double drx = r2x - r1x;
    double dry = r2y - r1y;
    double dvx = v2x - v1x;
    double dvy = v2y - v1y;
    double R = R1 + R2;

    double a = dvx*dvx + dvy*dvy;
    double b = 2.0* (drx*dvx + dry*dvy);
    double c = drx*drx + dry*dry - R*R;

    // (a = 0) => (v1 = v2) => (b = 0)
    if (a == 0.0) {
        // (c /= 0) => les deux objets ne se touche jamais
        // (c == 0) => les deux objets sont en contact permanent
        return -1.0;
    }

    double delta = b*b - 4.0 * a * c;

    if (delta < 0.0)
        return -1.0;

    double sqrt = std::sqrt(delta);

    double t1 = (-b - sqrt) / (2.0 * a);

    // todo: Si sa touche à t=0 on fait la collision oubien elle à déjà été faite ?
    // on gère toutes les collision qui ont : t < dt
    if (t1 >= 0.0)
        return t1;

    double t2 = (-b + sqrt) / (2.0 * a);

    if (t2 >= 0.0)
        return t2;

    return -1.0;
}
