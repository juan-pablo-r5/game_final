#include "escenanivel2.h"
#include <QFont>
#include <stdexcept>

void EscenaNivel2::inicializar()
{
    m_estado = EstadoNivel::Jugando;
    m_puntosGanados = 0;
    m_tiempoRestante = 90.0;
    m_integridadPalacio = 100;

    if (!m_escenario.cargarNivelPorNumero(2)) {
        throw std::runtime_error("No se pudo cargar el Nivel 2.");
    }

    posicionarSpiderEnInicio();
}

QPointF EscenaNivel2::posicionInicialSpider() const
{
    const double xCentro = (m_escenario.ancho() > 0) ? (m_escenario.ancho() * 0.5) : 100.0;
    return QPointF(xCentro, 50.0);
}

void EscenaNivel2::actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana)
{
    Q_UNUSED(altoVentana);

    avanzarSpiderYColisiones(dt, teclasActivas);
    actualizarCamara(anchoMundoVisible); // Sin scroll: m_camaraX = 0

    if (m_estado != EstadoNivel::Jugando) {
        return;
    }

    m_tiempoRestante -= dt;
    if (m_integridadPalacio <= 0) {
        m_estado = EstadoNivel::Derrota;
    } else if (m_tiempoRestante <= 0.0) {
        m_tiempoRestante = 0.0;
        m_puntosGanados = 1000;
        m_estado = EstadoNivel::Victoria;
    }
}

void EscenaNivel2::dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const
{
    Q_UNUSED(altoVentana);
    painter.save();

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(20, 28, nombreNivel());
    painter.drawText(anchoVentana - 420, 28,
                     QString("Puntos: %1  |  Palacio: %2%  |  Tiempo: %3s")
                         .arg(puntuacionTotal)
                         .arg(m_integridadPalacio)
                         .arg(static_cast<int>(m_tiempoRestante)));

    painter.restore();
}
