#include "escena.h"

#include <QColor>
#include <QDebug>
#include <QRect>
#include <QtGlobal>

void Escena::actualizar(double deltaTime) {
    Q_UNUSED(deltaTime);
}

void Escena::dibujar(QPainter &painter, int anchoVista, int altoVista) {
    painter.fillRect(0, 0, anchoVista, altoVista, Qt::black);

    const QImage &visual = m_escenario.mapaVisual();
    if (visual.isNull() || anchoVista <= 0 || altoVista <= 0) {
        return;
    }

    const int maxCamaraX = qMax(0, visual.width() - anchoVista);
    m_camaraX = qBound(0, m_camaraX, maxCamaraX);

    const int anchoVisible = qMin(anchoVista, visual.width() - m_camaraX);
    const int altoVisible = qMin(altoVista, visual.height());

    const QRect fuente(m_camaraX, 0, anchoVisible, altoVisible);
    const QRect destino(0, 0, anchoVisible, altoVisible);

    painter.drawImage(destino, visual, fuente);
}

void Escena::manejarTecla(QKeyEvent *event, int anchoVista) {
    if (!event) {
        return;
    }

    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        moverCamaraX(m_velocidadCamara, anchoVista);
    } else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        moverCamaraX(-m_velocidadCamara, anchoVista);
    }
}

void Escena::manejarClic(QMouseEvent *event) {
    if (!event || event->button() != Qt::LeftButton || m_escenario.mapaColision().isNull()) {
        return;
    }

    const int mundoX = event->pos().x() + m_camaraX;
    const int mundoY = event->pos().y();

    const bool suelo = esSueloEnMundo(mundoX, mundoY);
    const bool pared = esParedEnMundo(mundoX, mundoY);
    const QColor c = m_escenario.mapaColision().pixelColor(mundoX, mundoY);

    qDebug() << "[" << nombre() << "]"
             << "Mundo (" << mundoX << "," << mundoY << ")"
             << "| RGBA(" << c.red() << "," << c.green() << "," << c.blue() << "," << c.alpha() << ")"
             << "| esSuelo:" << suelo
             << "| esPared:" << pared;
}

Escenario &Escena::escenario() {
    return m_escenario;
}

const Escenario &Escena::escenario() const {
    return m_escenario;
}

int Escena::camaraX() const {
    return m_camaraX;
}

void Escena::setCamaraX(int x, int anchoVista) {
    if (anchoVista > 0 && !m_escenario.mapaVisual().isNull()) {
        const int maxCamaraX = qMax(0, m_escenario.mapaVisual().width() - anchoVista);
        m_camaraX = qBound(0, x, maxCamaraX);
    } else {
        m_camaraX = qMax(0, x);
    }
}

void Escena::moverCamaraX(int dx, int anchoVista) {
    setCamaraX(m_camaraX + dx, anchoVista);
}

bool Escena::esSueloEnMundo(int mundoX, int mundoY) const {
    return m_escenario.esSuelo(mundoX, mundoY);
}

bool Escena::esParedEnMundo(int mundoX, int mundoY) const {
    return m_escenario.esPared(mundoX, mundoY);
}