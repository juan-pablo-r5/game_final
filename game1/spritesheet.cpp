#include "spritesheet.h"
#include <QImage>
#include <QTransform>
#include <QtMath>
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

void SpriteSheet::eliminarColorFondo(const QColor &colorClave, int tolerancia)
{
    if (!m_imagenOriginal || m_imagenOriginal->isNull()) return;

    QImage img = m_imagenOriginal->toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        QRgb *linea = reinterpret_cast<QRgb *>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            QColor c = QColor::fromRgb(linea[x]);
            // Elimina tanto el verde brillante de caja como el verde oscuro de fondo de la hoja
            bool esVerdeFondo = (c.green() > 110 && c.red() < 90 && c.blue() < 90);
            int dist = qAbs(c.red() - colorClave.red())
                       + qAbs(c.green() - colorClave.green())
                       + qAbs(c.blue() - colorClave.blue());
            if (esVerdeFondo || dist <= tolerancia) {
                linea[x] = qRgba(0, 0, 0, 0);
            }
        }
    }
    *m_imagenOriginal = QPixmap::fromImage(img);
    *m_regionRecortada = *m_imagenOriginal;
}

void SpriteSheet::agregarAnimacion(const QRect &filaORect, unsigned int cantidadCuadros, unsigned int pasoX)
{
    m_animaciones.push_back(filaORect);
    m_conteoCuadros.push_back(cantidadCuadros > 0 ? cantidadCuadros : 1);
    m_pasosX.push_back(pasoX > 0 ? pasoX : m_anchoCuadro);
    m_contadorActual.push_back(0);
}

QPixmap SpriteSheet::obtenerCuadroAnimado(unsigned int indiceAnimacion, bool voltearHorizontal)
{
    if (indiceAnimacion >= static_cast<unsigned int>(m_animaciones.size()) || m_regionRecortada->isNull()) {
        return QPixmap();
    }

    const QRect &rectAnim = m_animaciones[indiceAnimacion];
    unsigned int frame = m_contadorActual[indiceAnimacion];
    unsigned int paso = m_pasosX[indiceAnimacion];

    QPixmap subRegion = m_regionRecortada->copy(rectAnim);
    QPixmap cuadro = subRegion.copy(static_cast<int>(frame * paso),
                                    0,
                                    static_cast<int>(m_anchoCuadro),
                                    static_cast<int>(m_altoCuadro));

    m_contadorActual[indiceAnimacion] = (frame + 1) % m_conteoCuadros[indiceAnimacion];

    if (voltearHorizontal) {
        cuadro = cuadro.transformed(QTransform().scale(-1.0, 1.0));
    }

    return cuadro.scaled(static_cast<int>(m_anchoCuadro * m_escala),
                         static_cast<int>(m_altoCuadro * m_escala),
                         Qt::KeepAspectRatio,
                         Qt::FastTransformation);
}

QPixmap SpriteSheet::obtenerImagenFija(const QRect &region, bool voltearHorizontal)
{
    if (m_regionRecortada->isNull()) {
        return QPixmap();
    }
    QPixmap cuadro = m_regionRecortada->copy(region);
    if (voltearHorizontal) {
        cuadro = cuadro.transformed(QTransform().scale(-1.0, 1.0));
    }
    return cuadro.scaled(static_cast<int>(m_anchoCuadro * m_escala),
                         static_cast<int>(m_altoCuadro * m_escala),
                         Qt::KeepAspectRatio,
                         Qt::FastTransformation);
}
