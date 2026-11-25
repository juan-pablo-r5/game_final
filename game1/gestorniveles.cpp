#include "gestorniveles.h"
#include "escenanivel1.h"
#include "escenanivel2.h"

GestorNiveles::GestorNiveles()
{
    cargarNivel(1);
}

GestorNiveles::~GestorNiveles()
{
    delete m_escenaActual;
    m_escenaActual = nullptr;
}

void GestorNiveles::cargarNivel(int numeroNivel)
{
    EscenaBase *nuevaEscena = nullptr;

    if (numeroNivel == 2) {
        nuevaEscena = new EscenaNivel2();
        m_nivelActual = 2;
    } else {
        nuevaEscena = new EscenaNivel1();
        m_nivelActual = 1;
    }

    delete m_escenaActual;
    m_escenaActual = nuevaEscena;
}

void GestorNiveles::reiniciarNivelActual()
{
    if (m_escenaActual) {
        m_escenaActual->inicializar();
    }
}
