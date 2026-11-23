#pragma once

#include "escena.h"

class EscenaNivel1 : public Escena {
public:
    EscenaNivel1() = default;

    bool cargar() override;
    void actualizar(double deltaTime) override;
    void dibujar(QPainter &painter, int anchoVista, int altoVista) override;
    void manejarTecla(QKeyEvent *event, int anchoVista) override;

    QString nombre() const override;
    int numeroNivel() const override;
};