#include "escenanivel1.h"

#include <QDebug>

bool EscenaNivel1::cargar() {
    m_camaraX = 0;
    return m_escenario.cargarNivelPorNumero(1);
}

void EscenaNivel1::actualizar(double deltaTime) {
    Escena::actualizar(deltaTime);
    // Actualizacion de cosas para el nivel 1
}

void EscenaNivel1::dibujar(QPainter &painter, int anchoVista, int altoVista) {
    Escena::dibujar(painter, anchoVista, altoVista);
    // Dibujar cosas en el mapa del nivel 1
}

void EscenaNivel1::manejarTecla(QKeyEvent *event, int anchoVista) {
    Escena::manejarTecla(event, anchoVista);
}

QString EscenaNivel1::nombre() const {
    return "Nivel 1";
}

int EscenaNivel1::numeroNivel() const {
    return 1;
}