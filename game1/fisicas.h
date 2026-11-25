#ifndef FISICAS_H
#define FISICAS_H

#include <QPointF>

struct ParametrosPendulo {
    double longitud;        // Longitud efectiva (unidades de escena)
    double gravedad;        // Aceleracion gravitacional
    double amortiguamiento; // Coeficiente de amortiguamiento >= 0
    double anguloRad;       // Angulo actual respecto a la vertical (radianes)
    double velocidadAngular;// Velocidad angular actual (rad/s)
};

struct ParametrosResorte {
    double constanteK;      // Rigidez del resorte (k > 0)
    double amortiguamiento; // Coeficiente de amortiguamiento c >= 0
    double masa;            // Masa asociada (m > 0)
    double desplazamiento;  // x - x_equilibrio
    double velocidad;       // dx/dt
};

class FisicasParametrizables
{
public:
    // 1. Pendulo amortiguado (integracion semi-implicita estable)
    static void avanzarPenduloAmortiguado(ParametrosPendulo &p, double dt);
    static QPointF calcularPosicionPendulo(const QPointF &pivote, const ParametrosPendulo &p);

    // 2. Movimiento parabolico
    static void avanzarMovimientoParabolico(double &x, double &y,
                                            double &vx, double &vy,
                                            double gravedad, double dt);

    // 3. Resorte / muelle amortiguado
    static void avanzarResorteAmortiguado(ParametrosResorte &r, double dt);

    // 4. Onda de choque radial por explosion
    static QPointF calcularImpulsoOndaChoque(const QPointF &centroExplosion,
                                             const QPointF &posicionObjeto,
                                             double radioMaximo,
                                             double intensidadMaxima,
                                             double masaObjeto);
};

#endif // FISICAS_H
