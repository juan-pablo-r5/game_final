#include "fisicas.h"
#include <QtMath>

void FisicasParametrizables::avanzarPenduloAmortiguado(ParametrosPendulo &p, double dt)
{
    if (dt <= 0.0 || p.longitud <= 0.0) return;

    // alpha = -(g / L) * sin(theta) - b * omega
    double aceleracionAngular = -(p.gravedad / p.longitud) * qSin(p.anguloRad)
                                - p.amortiguamiento * p.velocidadAngular;
    p.velocidadAngular += aceleracionAngular * dt;
    p.anguloRad += p.velocidadAngular * dt;
}

QPointF FisicasParametrizables::calcularPosicionPendulo(const QPointF &pivote,
                                                        const ParametrosPendulo &p)
{
    double x = pivote.x() + p.longitud * qSin(p.anguloRad);
    double y = pivote.y() + p.longitud * qCos(p.anguloRad);
    return QPointF(x, y);
}

void FisicasParametrizables::avanzarMovimientoParabolico(double &x, double &y,
                                                         double &vx, double &vy,
                                                         double gravedad, double dt)
{
    if (dt <= 0.0) return;

    vy += gravedad * dt;
    x += vx * dt;
    y += vy * dt;
}

void FisicasParametrizables::avanzarResorteAmortiguado(ParametrosResorte &r, double dt)
{
    if (dt <= 0.0 || r.masa <= 0.0) return;

    // m * a = -k * x - c * v
    double aceleracion = (-r.constanteK * r.desplazamiento - r.amortiguamiento * r.velocidad) / r.masa;
    r.velocidad += aceleracion * dt;
    r.desplazamiento += r.velocidad * dt;
}

QPointF FisicasParametrizables::calcularImpulsoOndaChoque(const QPointF &centroExplosion,
                                                          const QPointF &posicionObjeto,
                                                          double radioMaximo,
                                                          double intensidadMaxima,
                                                          double masaObjeto)
{
    if (radioMaximo <= 0.0 || masaObjeto <= 0.0) {
        return QPointF(0.0, 0.0);
    }

    double dx = posicionObjeto.x() - centroExplosion.x();
    double dy = posicionObjeto.y() - centroExplosion.y();
    double distancia = qSqrt(dx * dx + dy * dy);

    if (distancia >= radioMaximo) {
        return QPointF(0.0, 0.0);
    }

    // Atenuacion lineal suave segun cercania al epicentro
    double factor = (radioMaximo - distancia) / radioMaximo;
    double nx = (distancia > 1e-5) ? (dx / distancia) : 0.0;
    double ny = (distancia > 1e-5) ? (dy / distancia) : -1.0;

    double magnitudImpulso = (intensidadMaxima * factor) / masaObjeto;
    return QPointF(nx * magnitudImpulso, ny * magnitudImpulso);
}
