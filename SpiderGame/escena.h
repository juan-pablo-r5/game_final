#pragma once

#include "escenario.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QString>

class Escena {
public:
    Escena() = default;
    virtual ~Escena() = default;

    // Interfaz común para todas las escenas
    virtual bool cargar() = 0;
    virtual void actualizar(double deltaTime);
    virtual void dibujar(QPainter &painter, int anchoVista, int altoVista);

    virtual void manejarTecla(QKeyEvent *event, int anchoVista);
    virtual void manejarClic(QMouseEvent *event);

    virtual QString nombre() const = 0;
    virtual int numeroNivel() const = 0;

    // Acceso a escenario, cámara y colisiones
    Escenario &escenario();
    const Escenario &escenario() const;

    int camaraX() const;
    void setCamaraX(int x, int anchoVista = 0);
    void moverCamaraX(int dx, int anchoVista);

    bool esSueloEnMundo(int mundoX, int mundoY) const;
    bool esParedEnMundo(int mundoX, int mundoY) const;

protected:
    Escenario m_escenario;
    int m_camaraX = 0;
    int m_velocidadCamara = 40;
};