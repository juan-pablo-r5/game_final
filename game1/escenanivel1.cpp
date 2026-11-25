#include "escenanivel1.h"
#include <QFont>
#include <stdexcept>

void EscenaNivel1::inicializar()
{
    m_estado = EstadoNivel::Jugando;
    m_puntosGanados = 0;
    m_tiempoRestante = 180.0;

    if (!m_escenario.cargarNivelPorNumero(1)) {
        throw std::runtime_error("No se pudo cargar el Nivel 1.");
    }

    m_metaX = qMax(200.0, static_cast<double>(m_escenario.ancho() - 120));
    posicionarSpiderEnInicio();
}

void EscenaNivel1::actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana)
{
    Q_UNUSED(altoVentana);

    avanzarSpiderYColisiones(dt, teclasActivas);
    actualizarCamara(anchoMundoVisible);

    if (m_estado != EstadoNivel::Jugando) {
        return;
    }

    m_tiempoRestante -= dt;
    if (m_tiempoRestante <= 0.0) {
        m_tiempoRestante = 0.0;
        m_estado = EstadoNivel::Derrota;
        return;
    }

    if (m_spiderX >= m_metaX) {
        m_puntosGanados = 500;
        m_estado = EstadoNivel::Victoria;
    }
}

void EscenaNivel1::dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const
{
    Q_UNUSED(altoVentana);
    painter.save();

    const int totalSeg = static_cast<int>(m_tiempoRestante);
    const int minutos = totalSeg / 60;
    const int segundos = totalSeg % 60;

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(20, 28, nombreNivel());
    painter.drawText(anchoVentana - 360, 28,
                     QString("Puntos: %1  |  Tiempo: %2:%3")
                         .arg(puntuacionTotal)
                         .arg(minutos, 2, 10, QChar('0'))
                         .arg(segundos, 2, 10, QChar('0')));

    painter.restore();
}
