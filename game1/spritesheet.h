#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <QColor>
#include <QPixmap>
#include <QRect>
#include <QString>
#include <QVector>

class SpriteSheet
{
public:
    explicit SpriteSheet(const QString &rutaRecurso, unsigned int escala = 1);
    ~SpriteSheet();

    void configurarTamanoCuadro(unsigned int ancho, unsigned int alto);
    void recortarRegion(const QRect &region);
    void agregarAnimacion(const QRect &filaORect, unsigned int cantidadCuadros, unsigned int pasoX = 0);

    // Convierte en transparente el fondo verde/solido de los JPG
    void eliminarColorFondo(const QColor &colorClave = QColor(0, 255, 0), int tolerancia = 95);

    QPixmap obtenerCuadroAnimado(unsigned int indiceAnimacion, bool voltearHorizontal = false);
    QPixmap obtenerImagenFija(const QRect &region, bool voltearHorizontal = false);

private:
    QPixmap *m_imagenOriginal;
    QPixmap *m_regionRecortada;
    QVector<QRect> m_animaciones;
    QVector<unsigned int> m_conteoCuadros;
    QVector<unsigned int> m_pasosX;
    QVector<unsigned int> m_contadorActual;

    unsigned int m_anchoCuadro;
    unsigned int m_altoCuadro;
    unsigned int m_escala;
};

#endif // SPRITESHEET_H

