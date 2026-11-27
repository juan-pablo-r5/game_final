#ifndef GESTORNIVELES_H
#define GESTORNIVELES_H

#include "escenabase.h"

class GestorNiveles
{
public:
    GestorNiveles();
    ~GestorNiveles();

    void cargarNivel(int numeroNivel);
    void reiniciarNivelActual();
    EscenaBase* escenaActual() const { return m_escenaActual; }
    int numeroNivelActual() const { return m_nivelActual; }

private:
    EscenaBase *m_escenaActual = nullptr;
    int m_nivelActual = 1;
};

#endif // GESTORNIVELES_H
