#ifndef ESCENANIVEL2_H
#define ESCENANIVEL2_H

#include "escenabase.h"
#include "spritesheet.h"
#include <QPixmap>
#include <QVector>

class EscenaNivel2 : public EscenaBase
{
public:
    EscenaNivel2() = default;
    ~EscenaNivel2() override;

    void inicializar() override;
    void actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana) override;
    void dibujar(QPainter &painter, double escala, int offsetX, int anchoMundoVisible, int altoVentana) override;
    void dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const override;

    bool tieneScrollHorizontal() const override { return false; }
    QPointF posicionInicialSpider() const override;
    QString nombreNivel() const override { return "Nivel 2: Defensa del Palacio"; }

private:
    enum class EstadoAgente {
        Patrulla,
        PerseguirSpider
    };

    struct EnemigoAgente {
        double x = 200.0;
        double y = 50.0;
        double vx = 0.0;
        double vy = 0.0;
        int ancho = 28;
        int alto = 40;
        int golpesRecibidos = 0;
        double tiempoRalentizado = 0.0;
        bool vivo = true;
        bool enSuelo = false;
        bool mirandoIzquierda = true;
        int dirPatrulla = -1;
        double tiempoAnim = 0.0;
        double tiempoRespawn = 0.0;
        QPixmap frame;
        EstadoAgente estado = EstadoAgente::Patrulla;
    };

    struct TelaranaProyectil {
        double x = 0.0;
        double y = 0.0;
        double vx = 0.0;
        double vy = 0.0;
        int ancho = 20;
        int alto = 20;
        double tiempoVida = 1.25;
        bool activa = true;
    };

    void inicializarSpriteTelarana();
    void inicializarEnemigos(int cantidadTotal);
    void spawnEnemigo(int indice, double posXInicial);
    void gestionarOleadasEnemigos(double dt);
    void dispararTelarana();
    void actualizarTelaranas(double dt);
    void actualizarEnemigos(double dt);
    void dibujarTelaranas(QPainter &painter, double escala, int offsetX) const;
    void dibujarEnemigos(QPainter &painter, double escala, int offsetX, int altoVentana) const;

    double m_tiempoRestante = 90.0;
    int m_integridadPalacio = 100;
    int m_vidasSpider = 3;
    double m_invencibilidadSpider = 0.0;
    double m_cooldownDanoPalacio = 0.0;
    double m_cooldownDisparo = 0.0;
    double m_cooldownSpawnExtra = 0.0;

    QPixmap m_spriteTelarana;
    bool m_tieneSpriteTelaranaValido = false;
    QVector<TelaranaProyectil> m_telaranas;

    SpriteSheet *m_spriteEnemigo = nullptr;
    QVector<EnemigoAgente> m_enemigos;
    int m_maxEnemigosActivos = 2;
};

#endif // ESCENANIVEL2_H
