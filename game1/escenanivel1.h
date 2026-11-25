#ifndef ESCENANIVEL1_H
#define ESCENANIVEL1_H

#include "escenabase.h"

class EscenaNivel1 : public EscenaBase
{
public:
    EscenaNivel1() = default;

    void inicializar() override;
    void actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana) override;
    void dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const override;

    bool tieneScrollHorizontal() const override { return true; }
    QPointF posicionInicialSpider() const override { return QPointF(100.0, 50.0); }
    QString nombreNivel() const override { return "Nivel 1: El Bogotazo"; }

private:
    double m_tiempoRestante = 180.0; // 3 minutos
    double m_metaX = 0.0;
};

#endif // ESCENANIVEL1_H
