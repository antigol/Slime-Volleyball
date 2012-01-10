#include "world.h"
#include <cmath>

World::World()
{
}

void World::move(double dt, World::Movements p0, World::Movements p1)
{
    Movements playersMov[2] = {p0, p1};

    // déplace les objets de dt
    QPointF dx(dt * _pSpeed, 0);

    QPointF playersSpeed[2];

    for (int i = 0; i < 2; ++i) {
        if (playersMov[i] & Left) {
            _playersPos[i] -= dx;
            playersSpeed[i].rx() = -_pSpeed;
        }
        if (playersMov[i] & Right) {
            _playersPos[i] += dx;
            playersSpeed[i].rx() = _pSpeed;
        }
        if (playersMov[i] & Up) {
            // se prépare à voler
            _tvol[i] = 0;
        }

        if (_tvol[0] >= 0) {
            _tvol[0] += dt;
            double y = _pSpeed * _tvol[0] - 0.5 * _gValue * _tvol[0] * _tvol[0];
            double vy = _pSpeed - _gValue * _tvol[0];
            if (y <= 0) {
                y = vy = 0;
                _tvol[0] = -1;
            }
            _playersPos[i].ry() = y;
            playersSpeed[i].ry() = vy;
        }

    }

    _ballTVol += dt;
    // calcul de la position exacte (parabole)
    _ballActualPos.rx() = _ballPosInit.x() + _ballSpeedInit.x() * _ballTVol;
    _ballActualPos.ry() = _ballPosInit.y() + _ballSpeedInit.y() * _ballTVol - 0.5 * _gValue * _ballTVol * _ballTVol;

    QPointF ballSpeed;

    // collisions
    for (int i = 0; i < 2; ++i) {
        QPointF u = _ballActualPos - _playersPos[i];
        double dist = length(u);
        u /= dist;

        if (dist <= WORLD_BALL_RADIUS + WORLD_SLIME_RADIUS) {
            // referentiel : playeri
            QPointF s = ballSpeed - playersSpeed[i];
            double factor = dotProduct(s, u);
            // Le facteur doit être negatif, s'il est positif c'est un bug
            if (factor < 0) {
                // debond sans perte d'energie
                s -= 2.0 * factor * u;
                // referentiel : world
                _ballSpeedInit = s + playersSpeed[i];
                _ballPosInit = _ballActualPos;
                _ballTVol = 0;
            }
        }

    }
}

double World::length(QPointF p)
{
    return std::sqrt(p.x()*p.x() + p.y()*p.y());
}

double World::dotProduct(QPointF p1, QPointF p2)
{
    return p1.x()*p2.x() + p1.y()*p2.y();
}

double World::playerSpeed() const
{
    return _pSpeed;
}
