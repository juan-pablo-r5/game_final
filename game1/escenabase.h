#ifndef ESCENABASE_H
#define ESCENABASE_H

#include "escenario.h"
#include "spritesheet.h"
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <QSet>
#include <QString>

enum class EstadoNivel {
    Jugando,
    Victoria,
    Derrota
};

class EscenaBase
{
public:
    virtual ~EscenaBase() = default;

    virtual void inicializar() = 0;
    virtual void actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana) = 0;
    virtual void dibujar(QPainter &painter, double escala, int offsetX, int anchoMundoVisible, int altoVentana);
    virtual void dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const = 0;

    virtual bool tieneScrollHorizontal() const = 0;
    virtual QPointF posicionInicialSpider() const = 0;
    virtual QString nombreNivel() const = 0;

    void asignarSpriteSpider(SpriteSheet *sprite);
    void posicionarSpiderEnInicio();

    Escenario& escenario() { return m_escenario; }
    const Escenario& escenario() const { return m_escenario; }
    int camaraX() const { return m_camaraX; }
    EstadoNivel estado() const { return m_estado; }
    int puntosGanados() const { return m_puntosGanados; }

protected:
    void avanzarSpiderYColisiones(double dt, const QSet<int> &teclasActivas);
    void actualizarAnimacionSpider(double dt, double dirX);
    void actualizarCamara(int anchoMundoVisible);

    Escenario m_escenario;
    EstadoNivel m_estado = EstadoNivel::Jugando;
    int m_puntosGanados = 0;

    double m_spiderX = 100.0;
    double m_spiderY = 50.0;
    double m_vx = 0.0;
    double m_vy = 0.0;
    int m_anchoSpider = 26;
    int m_altoSpider = 40;
    bool m_enSuelo = false;
    bool m_intentandoMoverse = false;
    bool m_mirandoIzquierda = false;

    SpriteSheet *m_spriteSpider = nullptr;
    QPixmap m_frameActualSpider;
    double m_tiempoAnimacion = 0.0;

    int m_camaraX = 0;
};

#endif // ESCENABASE_H
