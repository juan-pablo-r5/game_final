#ifndef ESCENANIVEL1_H
#define ESCENANIVEL1_H

#include "escenabase.h"
#include <QPixmap>
#include <QVector>

class EscenaNivel1 : public EscenaBase
{
public:
    EscenaNivel1() = default;

    void inicializar() override;
    void actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana) override;
    void dibujar(QPainter &painter, double escala, int offsetX, int anchoMundoVisible, int altoVentana) override;
    void dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const override;

    bool tieneScrollHorizontal() const override { return true; }
    QPointF posicionInicialSpider() const override { return QPointF(100.0, 50.0); }
    QString nombreNivel() const override { return "Nivel 1: El Bogotazo"; }

private:
    struct Ciudadano {
        double x = 0.0;
        double y = 0.0;
        double xMin = 0.0;
        double xMax = 0.0;
        double vx = 45.0;
        int ancho = 36;
        int alto = 52;
        bool caminando = true;
        bool mirandoIzquierda = false;
        bool salvado = false;

        QPixmap hoja;
        QVector<QRect> cuadrosCaminar;
        int indiceCuadro = 0;
        double tiempoAnim = 0.0;
    };

    QPixmap cargarPixmapAsset(const QString &subruta) const;
    void cargarHojasCiudadanos();
    void configurarCuadrosDesdeHoja(Ciudadano &c, int tipoIdx, int cuadrosEstimados) const;
    void ubicarCiudadanoEnSuelo(Ciudadano &c) const;
    void inicializarCiudadanos();
    void actualizarCiudadanos(double dt);
    void dibujarCiudadanos(QPainter &painter, double escala, int offsetX) const;

    double m_tiempoRestante = 180.0; // 3 minutos
    int m_ciudadanosSalvados = 0;
    QVector<QPixmap> m_hojasTipos;
    QVector<int> m_cuadrosPorTipo;
    QVector<Ciudadano> m_ciudadanos;
};

#endif // ESCENANIVEL1_H