#include "spritesheet.h"
#include <stdexcept>

SpriteSheet::SpriteSheet(const QString &rutaRecurso, unsigned int escala)
    : m_imagenOriginal(new QPixmap())
    , m_regionRecortada(new QPixmap())
    , m_anchoCuadro(16)
    , m_altoCuadro(16)
    , m_escala(escala > 0 ? escala : 1)
{
    if (!m_imagenOriginal->load(rutaRecurso)) {
        delete m_imagenOriginal;
        delete m_regionRecortada;
        throw std::runtime_error("Error al cargar el sprite sheet: " + rutaRecurso.toStdString());
    }
    *m_regionRecortada = *m_imagenOriginal;
}

SpriteSheet::~SpriteSheet()
{
    delete m_imagenOriginal;
    delete m_regionRecortada;
}

void SpriteSheet::configurarTamanoCuadro(unsigned int ancho, unsigned int alto)
{
    m_anchoCuadro = ancho;
    m_altoCuadro = alto;
}

void SpriteSheet::recortarRegion(const QRect &region)
{
    if (!region.isEmpty() && m_imagenOriginal) {
        *m_regionRecortada = m_imagenOriginal->copy(region);
    }
}

void SpriteSheet::agregarAnimacion(const QRect &filaORect, unsigned int cantidadCuadros)
{
    m_animaciones.push_back(filaORect);
    m_conteoCuadros.push_back(cantidadCuadros > 0 ? cantidadCuadros : 1);
    m_contadorActual.push_back(0);
}

QPixmap SpriteSheet::obtenerCuadroAnimado(unsigned int indiceAnimacion)
{
    if (indiceAnimacion >= static_cast<unsigned int>(m_animaciones.size()) || m_regionRecortada->isNull()) {
        return QPixmap();
    }

    const QRect &rectAnim = m_animaciones[indiceAnimacion];
    unsigned int frame = m_contadorActual[indiceAnimacion];

    QPixmap subRegion = m_regionRecortada->copy(rectAnim);
    QPixmap cuadro = subRegion.copy(static_cast<int>(frame * m_anchoCuadro),
                                    0,
                                    static_cast<int>(m_anchoCuadro),
                                    static_cast<int>(m_altoCuadro));

    m_contadorActual[indiceAnimacion] = (frame + 1) % m_conteoCuadros[indiceAnimacion];

    return cuadro.scaled(static_cast<int>(m_anchoCuadro * m_escala),
                         static_cast<int>(m_altoCuadro * m_escala),
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
}

QPixmap SpriteSheet::obtenerImagenFija(const QRect &region)
{
    if (m_regionRecortada->isNull()) {
        return QPixmap();
    }
    return m_regionRecortada->copy(region).scaled(static_cast<int>(m_anchoCuadro * m_escala),
                                                  static_cast<int>(m_altoCuadro * m_escala),
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation);
}
