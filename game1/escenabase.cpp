#include "escenabase.h"
#include <QtMath>

void EscenaBase::asignarSpriteSpider(SpriteSheet *sprite)
{
    m_spriteSpider = sprite;
    if (m_spriteSpider) {
        m_spriteSpider->configurarTamanoCuadro(24, 34);
        m_frameActualSpider = m_spriteSpider->obtenerImagenFija(QRect(4, 14, 24, 34), m_mirandoIzquierda);
    }
}

void EscenaBase::posicionarSpiderEnInicio()
{
    m_camaraX = 0;
    const QPointF inicio = posicionInicialSpider();
    m_spiderX = inicio.x();
    m_spiderY = inicio.y();
    m_vx = 0.0;
    m_vy = 0.0;
    m_knockbackVx = 0.0;
    m_knockbackVy = 0.0;
    m_enSuelo = false;

    if (m_escenario.mapaColision().isNull()) {
        return;
    }

    const int xPrueba = qBound(0, static_cast<int>(m_spiderX) + 20, m_escenario.ancho() - 1);
    const int yInicioBusqueda = qMax(0, static_cast<int>(m_spiderY));
    for (int y = yInicioBusqueda; y < m_escenario.alto(); ++y) {
        if (m_escenario.esSuelo(xPrueba, y)) {
            m_spiderY = static_cast<double>(y - m_altoSpider);
            break;
        }
    }
}

void EscenaBase::aplicarEmpujeSpider(double empujeX, double empujeY)
{
    m_knockbackVx = empujeX;
    m_knockbackVy = empujeY;
}

void EscenaBase::avanzarSpiderYColisiones(double dt, const QSet<int> &teclasActivas)
{
    if (m_escenario.mapaColision().isNull()) {
        return;
    }

    const double velocidadHorizontal = 280.0;
    const double gravedad = 980.0;
    const double impulsoSalto = -520.0;
    const double maxVelocidadCaida = 750.0;

    // 1. Entrada horizontal
    double dirX = 0.0;
    if (teclasActivas.contains(Qt::Key_A) || teclasActivas.contains(Qt::Key_Left)) {
        dirX -= 1.0;
    }
    if (teclasActivas.contains(Qt::Key_D) || teclasActivas.contains(Qt::Key_Right)) {
        dirX += 1.0;
    }

    m_intentandoMoverse = (qAbs(dirX) > 0.01);
    m_vx = dirX * velocidadHorizontal;

    // 2. Impulso vertical de empuje o salto normal si esta en el suelo
    if (qAbs(m_knockbackVy) > 0.01) {
        m_vy = m_knockbackVy;
        m_enSuelo = false;
        m_knockbackVy = 0.0;
    } else if ((teclasActivas.contains(Qt::Key_W) ||
                teclasActivas.contains(Qt::Key_Up) ||
                teclasActivas.contains(Qt::Key_Space)) && m_enSuelo) {
        m_vy = impulsoSalto;
        m_enSuelo = false;
    }

    // 3. Avance horizontal en sub-pasos (maximo 2 px por paso), incluyendo empuje
    const double velTotalX = m_vx + m_knockbackVx;
    const double desplazamientoX = velTotalX * dt;
    const int pasosX = qMax(1, static_cast<int>(qCeil(qAbs(desplazamientoX) / 2.0)));
    const double pasoX = desplazamientoX / pasosX;
    const double limiteDer = static_cast<double>(qMax(0, m_escenario.ancho() - m_anchoSpider));

    for (int i = 0; i < pasosX; ++i) {
        const double candidatoX = qBound(0.0, m_spiderX + pasoX, limiteDer);
        const int xCheck = (pasoX > 0.0)
                               ? static_cast<int>(candidatoX + m_anchoSpider)
                               : static_cast<int>(candidatoX);

        const int yTop = static_cast<int>(m_spiderY + 4);
        const int yMid = static_cast<int>(m_spiderY + m_altoSpider * 0.5);
        const int yBot = static_cast<int>(m_spiderY + m_altoSpider - 4);

        const bool golpeaPared = (pasoX != 0.0) && (
                                     m_escenario.esPared(xCheck, yTop) ||
                                     m_escenario.esPared(xCheck, yMid) ||
                                     m_escenario.esPared(xCheck, yBot)
                                     );

        if (golpeaPared) {
            m_vx = 0.0;
            m_knockbackVx = 0.0;
            break;
        }
        m_spiderX = candidatoX;
    }

    // Amortiguar el empuje horizontal progresivamente
    if (qAbs(m_knockbackVx) > 0.5) {
        const double frenado = 900.0 * dt;
        if (m_knockbackVx > 0.0) {
            m_knockbackVx = qMax(0.0, m_knockbackVx - frenado);
        } else {
            m_knockbackVx = qMin(0.0, m_knockbackVx + frenado);
        }
    } else {
        m_knockbackVx = 0.0;
    }

    // 4. Gravedad acotada y avance vertical en sub-pasos (maximo 2 px por paso)
    m_vy = qMin(maxVelocidadCaida, m_vy + gravedad * dt);
    const double desplazamientoY = m_vy * dt;
    const int pasosY = qMax(1, static_cast<int>(qCeil(qAbs(desplazamientoY) / 2.0)));
    const double pasoY = desplazamientoY / pasosY;

    m_enSuelo = false;

    for (int i = 0; i < pasosY; ++i) {
        const double candidatoY = m_spiderY + pasoY;

        if (pasoY >= 0.0) {
            const int pieY = static_cast<int>(candidatoY + m_altoSpider);
            const int xPieIzq = static_cast<int>(m_spiderX + 6);
            const int xPieCentro = static_cast<int>(m_spiderX + m_anchoSpider * 0.5);
            const int xPieDer = static_cast<int>(m_spiderX + m_anchoSpider - 6);

            const bool tocaSuelo =
                m_escenario.esSuelo(xPieIzq, pieY) ||
                m_escenario.esSuelo(xPieCentro, pieY) ||
                m_escenario.esSuelo(xPieDer, pieY) ||
                m_escenario.esPared(xPieCentro, pieY);

            if (tocaSuelo) {
                m_spiderY = static_cast<double>(pieY - m_altoSpider);
                m_vy = 0.0;
                m_enSuelo = true;
                break;
            }
        }

        m_spiderY = candidatoY;
    }

    if (m_spiderY > m_escenario.alto()) {
        m_knockbackVx = 0.0;
        m_knockbackVy = 0.0;
        posicionarSpiderEnInicio();
    }

    actualizarAnimacionSpider(dt, dirX);
}

void EscenaBase::actualizarAnimacionSpider(double dt, double dirX)
{
    const bool direccionAnterior = m_mirandoIzquierda;
    if (dirX < -0.01) {
        m_mirandoIzquierda = true;
    } else if (dirX > 0.01) {
        m_mirandoIzquierda = false;
    }
    const bool cambioDireccion = (m_mirandoIzquierda != direccionAnterior);

    if (!m_spriteSpider) {
        return;
    }

    if (!m_enSuelo) {
        m_tiempoAnimacion = 0.0;
        if (m_frameActualSpider.isNull() || cambioDireccion) {
            m_frameActualSpider = m_spriteSpider->obtenerImagenFija(QRect(4, 14, 24, 34), m_mirandoIzquierda);
        }
    } else if (m_intentandoMoverse) {
        m_tiempoAnimacion += dt;
        if (m_tiempoAnimacion >= 0.09 || cambioDireccion) {
            if (m_tiempoAnimacion >= 0.09) {
                m_tiempoAnimacion = 0.0;
            }
            m_frameActualSpider = m_spriteSpider->obtenerCuadroAnimado(1, m_mirandoIzquierda);
        }
    } else {
        m_tiempoAnimacion = 0.0;
        if (m_frameActualSpider.isNull() || cambioDireccion) {
            m_frameActualSpider = m_spriteSpider->obtenerImagenFija(QRect(4, 14, 24, 34), m_mirandoIzquierda);
        }
    }
}

void EscenaBase::actualizarCamara(int anchoMundoVisible)
{
    if (!tieneScrollHorizontal()) {
        m_camaraX = 0;
        return;
    }

    const int maxCamaraX = qMax(0, m_escenario.ancho() - anchoMundoVisible);
    const int objetivoCamaraX = static_cast<int>(m_spiderX + m_anchoSpider * 0.5 - anchoMundoVisible * 0.4);
    m_camaraX = qBound(0, objetivoCamaraX, maxCamaraX);
}

void EscenaBase::dibujar(QPainter &painter, double escala, int offsetX, int anchoMundoVisible, int altoVentana)
{
    const QImage &visual = m_escenario.mapaVisual();
    if (visual.isNull() || visual.height() <= 0) {
        return;
    }

    const int maxCamaraX = qMax(0, visual.width() - anchoMundoVisible);
    m_camaraX = qBound(0, m_camaraX, maxCamaraX);

    const QRect fuente(m_camaraX, 0, anchoMundoVisible, visual.height());
    const int anchoDestino = static_cast<int>(anchoMundoVisible * escala);
    const QRect destino(offsetX, 0, anchoDestino, altoVentana);
    painter.drawImage(destino, visual, fuente);

    const int screenX = offsetX + static_cast<int>((m_spiderX - m_camaraX) * escala);
    const int screenY = static_cast<int>(m_spiderY * escala);
    const int screenW = qMax(16, static_cast<int>(m_anchoSpider * escala));
    const int screenH = qMax(24, static_cast<int>(m_altoSpider * escala));

    if (!m_frameActualSpider.isNull()) {
        painter.drawPixmap(QRect(screenX, screenY, screenW, screenH), m_frameActualSpider);
    } else {
        painter.setBrush(QColor(210, 30, 45));
        painter.drawRect(screenX, screenY, screenW, screenH);
    }
}
