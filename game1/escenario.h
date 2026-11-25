#pragma once

#include <QImage>
#include <QString>

class Escenario {
public:
    Escenario() = default;

    bool cargarNivel(const QString& rutaVisual, const QString& rutaColision);
    bool cargarNivelPorNumero(int numeroNivel);

    int nivelActual() const;
    int ancho() const;
    int alto() const;

    const QImage& mapaVisual() const;
    const QImage& mapaColision() const;

    bool esSuelo(int x, int y) const;
    bool esPared(int x, int y) const;

private:
    bool dentroDeLimites(int x, int y) const;

    QImage m_visual;
    QImage m_colision;
    int m_nivelActual = 0;
};