#ifndef ESCENANIVEL2_H
#define ESCENANIVEL2_H

#include "escenabase.h"

class EscenaNivel2 : public EscenaBase
{
public:
    EscenaNivel2() = default;

    void inicializar() override;
    void actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana) override;
    void dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const override;

    bool tieneScrollHorizontal() const override { return false; }
    QPointF posicionInicialSpider() const override;
    QString nombreNivel() const override { return "Nivel 2: Defensa del Palacio"; }

private:
    double m_tiempoRestante = 90.0;
    int m_integridadPalacio = 100;
};

#endif // ESCENANIVEL2_H
