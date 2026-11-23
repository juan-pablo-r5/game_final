#include "escenanivel2.h"

#include <QDebug>

bool EscenaNivel2::cargar() {
    m_camaraX = 0;
    return m_escenario.cargarNivelPorNumero(2);
}

void EscenaNivel2::actualizar(double deltaTime) {
    Escena::actualizar(deltaTime);
    // Actualizacion especifica nivel 2
}

void EscenaNivel2::dibujar(QPainter &painter, int anchoVista, int altoVista) {
    Escena::dibujar(painter, anchoVista, altoVista);
    // Cosas extras sobre el nivel2
}

void EscenaNivel2::manejarTecla(QKeyEvent *event, int anchoVista) {
    Escena::manejarTecla(event, anchoVista);
}

QString EscenaNivel2::nombre() const {
    return "Nivel 2";
}

int EscenaNivel2::numeroNivel() const {
    return 2;
}