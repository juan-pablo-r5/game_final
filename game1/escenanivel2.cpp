#include "escenanivel2.h"
#include <QColor>
#include <QDebug>
#include <QFont>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QtMath>
#include <stdexcept>

EscenaNivel2::~EscenaNivel2()
{
    delete m_spriteEnemigo;
    m_spriteEnemigo = nullptr;
}

void EscenaNivel2::inicializar()
{
    m_estado = EstadoNivel::Jugando;
    m_puntosGanados = 0;
    m_tiempoRestante = 90.0;
    m_integridadPalacio = 100;
    m_vidasSpider = 3;
    m_invencibilidadSpider = 0.0;
    m_cooldownDanoPalacio = 0.0;
    m_cooldownDisparo = 0.0;
    m_cooldownSpawnExtra = 8.0;
    m_maxEnemigosActivos = 2;
    aplicarEmpujeSpider(0.0, 0.0);
    m_telaranas.clear();

    if (!m_escenario.cargarNivelPorNumero(2)) {
        throw std::runtime_error("No se pudo cargar el Nivel 2.");
    }

    posicionarSpiderEnInicio();
    inicializarSpriteTelarana();
    inicializarEnemigos(4);
}

void EscenaNivel2::inicializarSpriteTelarana()
{
    m_spriteTelarana = QPixmap();
    m_tieneSpriteTelaranaValido = false;

    if (!m_spriteSpider) {
        return;
    }

    QPixmap recorte = m_spriteSpider->obtenerImagenFija(QRect(4, 14, 24, 34), false);
    if (recorte.isNull()) {
        return;
    }

    QImage img = recorte.toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor c = img.pixelColor(x, y);
            if (c.red() > 235 && c.green() > 235 && c.blue() > 235) {
                c.setAlpha(0);
                img.setPixelColor(x, y, c);
            }
        }
    }

    m_spriteTelarana = QPixmap::fromImage(img);
    m_tieneSpriteTelaranaValido = !m_spriteTelarana.isNull();
}

void EscenaNivel2::spawnEnemigo(int indice, double posXInicial)
{
    if (indice < 0 || indice >= m_enemigos.size()) {
        return;
    }

    EnemigoAgente &e = m_enemigos[indice];
    e.vivo = true;
    e.golpesRecibidos = 0;
    e.tiempoRalentizado = 0.0;
    e.x = qBound(40.0, posXInicial, static_cast<double>(qMax(60, m_escenario.ancho() - e.ancho - 20)));
    e.y = 50.0;
    e.vx = 0.0;
    e.vy = 0.0;
    e.enSuelo = false;
    e.dirPatrulla = (e.x >= m_spiderX) ? -1 : 1;
    e.mirandoIzquierda = (e.dirPatrulla < 0);
    e.tiempoAnim = 0.0;
    e.tiempoRespawn = 0.0;
    e.estado = EstadoAgente::Patrulla;

    if (m_spriteEnemigo) {
        e.frame = m_spriteEnemigo->obtenerCuadroAnimado(0, e.mirandoIzquierda);
    }

    if (!m_escenario.mapaColision().isNull()) {
        const int xPrueba = qBound(0, static_cast<int>(e.x) + 10, m_escenario.ancho() - 1);
        for (int y = 50; y < m_escenario.alto(); ++y) {
            if (m_escenario.esSuelo(xPrueba, y)) {
                e.y = static_cast<double>(y - e.alto);
                break;
            }
        }
    }
}

void EscenaNivel2::inicializarEnemigos(int cantidadTotal)
{
    try {
        delete m_spriteEnemigo;
        m_spriteEnemigo = nullptr;

        m_spriteEnemigo = new SpriteSheet(":/sprites/enemigo1.png", 1);
        m_spriteEnemigo->eliminarColorFondo();
        m_spriteEnemigo->configurarTamanoCuadro(32, 36);
        m_spriteEnemigo->agregarAnimacion(QRect(4, 4, 192, 36), 6, 32);
    } catch (const std::exception &e) {
        qWarning() << "No se pudo cargar enemigo1.png:" << e.what();
        delete m_spriteEnemigo;
        m_spriteEnemigo = nullptr;
    }

    m_enemigos.clear();
    m_enemigos.resize(cantidadTotal);

    const double anchoMapa = static_cast<double>(qMax(400, m_escenario.ancho()));
    for (int i = 0; i < cantidadTotal; ++i) {
        const double xSpawn = anchoMapa * (0.55 + 0.12 * i);
        spawnEnemigo(i, xSpawn);
        if (i >= m_maxEnemigosActivos) {
            m_enemigos[i].vivo = false;
            m_enemigos[i].tiempoRespawn = 2.0 + i * 2.5;
        }
    }
}

void EscenaNivel2::gestionarOleadasEnemigos(double dt)
{
    if (m_tiempoRestante <= 60.0 && m_maxEnemigosActivos < 3) {
        m_maxEnemigosActivos = 3;
    }
    if (m_tiempoRestante <= 30.0 && m_maxEnemigosActivos < 4) {
        m_maxEnemigosActivos = 4;
    }

    m_cooldownSpawnExtra -= dt;

    int vivosActuales = 0;
    for (const EnemigoAgente &e : m_enemigos) {
        if (e.vivo) {
            ++vivosActuales;
        }
    }

    if (vivosActuales < m_maxEnemigosActivos && m_cooldownSpawnExtra <= 0.0) {
        for (int i = 0; i < m_enemigos.size(); ++i) {
            if (!m_enemigos[i].vivo && m_enemigos[i].tiempoRespawn <= 0.0) {
                const double offsetSpawn = (i % 2 == 0)
                                               ? static_cast<double>(m_escenario.ancho() - 120)
                                               : static_cast<double>(m_escenario.ancho() * 0.72);
                spawnEnemigo(i, offsetSpawn);
                m_cooldownSpawnExtra = 1.6;
                break;
            }
        }
    }
}

QPointF EscenaNivel2::posicionInicialSpider() const
{
    const double xCentro = (m_escenario.ancho() > 0) ? (m_escenario.ancho() * 0.5) : 100.0;
    return QPointF(xCentro, 50.0);
}

void EscenaNivel2::dispararTelarana()
{
    if (m_cooldownDisparo > 0.0 || m_estado != EstadoNivel::Jugando) {
        return;
    }

    m_cooldownDisparo = 0.30;

    TelaranaProyectil web;
    const double velocidadWeb = 560.0;
    const double dir = m_mirandoIzquierda ? -1.0 : 1.0;

    web.x = m_mirandoIzquierda
                ? (m_spiderX - web.ancho)
                : (m_spiderX + m_anchoSpider);
    web.y = m_spiderY + m_altoSpider * 0.35;
    web.vx = dir * velocidadWeb + m_vx * 0.25;
    web.vy = -85.0; // Pequeño impulso inicial hacia arriba antes de la gravedad
    web.tiempoVida = 1.25;
    web.activa = true;

    m_telaranas.append(web);
}

void EscenaNivel2::actualizarTelaranas(double dt)
{
    if (m_cooldownDisparo > 0.0) {
        m_cooldownDisparo = qMax(0.0, m_cooldownDisparo - dt);
    }

    const double gravedadTelarana = 190.0;

    for (int i = 0; i < m_telaranas.size(); ++i) {
        TelaranaProyectil &w = m_telaranas[i];
        if (!w.activa) {
            continue;
        }

        // Fisica balistica suave de la telarana
        w.tiempoVida -= dt;
        w.vy += gravedadTelarana * dt;
        w.x += w.vx * dt;
        w.y += w.vy * dt;

        if (w.tiempoVida <= 0.0 ||
            w.x < -40.0 || w.x > m_escenario.ancho() + 40.0 ||
            w.y > m_escenario.alto() + 40.0) {
            w.activa = false;
            continue;
        }

        const int checkX = static_cast<int>(w.x + w.ancho * 0.5);
        const int checkY = static_cast<int>(w.y + w.alto * 0.5);
        if (!m_escenario.mapaColision().isNull() &&
            (m_escenario.esPared(checkX, checkY) || m_escenario.esSuelo(checkX, checkY))) {
            w.activa = false;
            continue;
        }

        // Impacto contra enemigo: 1er disparo ralentiza; 2do disparo elimina
        const QRectF rectWeb(w.x, w.y, w.ancho, w.alto);
        for (int j = 0; j < m_enemigos.size(); ++j) {
            EnemigoAgente &e = m_enemigos[j];
            if (!e.vivo) {
                continue;
            }
            const QRectF rectEnemigo(e.x, e.y, e.ancho, e.alto);
            if (rectWeb.intersects(rectEnemigo)) {
                w.activa = false;
                e.golpesRecibidos += 1;

                if (e.golpesRecibidos >= 2) {
                    e.vivo = false;
                    e.tiempoRespawn = 3.0;
                    m_puntosGanados += 150;
                } else {
                    // Primer impacto: ralentiza y aplica un leve retroceso
                    e.tiempoRalentizado = 2.2;
                    e.vx = (w.vx >= 0.0) ? 70.0 : -70.0;
                    m_puntosGanados += 40;
                }
                break;
            }
        }
    }

    for (int i = m_telaranas.size() - 1; i >= 0; --i) {
        if (!m_telaranas[i].activa) {
            m_telaranas.removeAt(i);
        }
    }
}

void EscenaNivel2::actualizarEnemigos(double dt)
{
    if (m_escenario.mapaColision().isNull()) {
        return;
    }

    if (m_cooldownDanoPalacio > 0.0) {
        m_cooldownDanoPalacio = qMax(0.0, m_cooldownDanoPalacio - dt);
    }
    if (m_invencibilidadSpider > 0.0) {
        m_invencibilidadSpider = qMax(0.0, m_invencibilidadSpider - dt);
    }

    gestionarOleadasEnemigos(dt);

    const double velocidadBase = 145.0;
    const double gravedad = 980.0;
    const double maxVelocidadCaida = 750.0;
    const double radioDeteccionSpider = 260.0;
    const QRectF rectSpider(m_spiderX, m_spiderY, m_anchoSpider, m_altoSpider);

    for (int idx = 0; idx < m_enemigos.size(); ++idx) {
        EnemigoAgente &e = m_enemigos[idx];

        if (!e.vivo) {
            if (e.tiempoRespawn > 0.0) {
                e.tiempoRespawn = qMax(0.0, e.tiempoRespawn - dt);
            }
            continue;
        }

        if (e.tiempoRalentizado > 0.0) {
            e.tiempoRalentizado = qMax(0.0, e.tiempoRalentizado - dt);
        }

        const double factorVelocidad = (e.tiempoRalentizado > 0.0) ? 0.40 : 1.0;
        const double velEnemigo = (velocidadBase + idx * 14.0) * factorVelocidad;
        const double distanciaSpider = qAbs(m_spiderX - e.x);
        double dirX = 0.0;

        if (distanciaSpider <= radioDeteccionSpider) {
            e.estado = EstadoAgente::PerseguirSpider;
            if (m_spiderX < e.x - 6.0) {
                dirX = -1.0;
            } else if (m_spiderX > e.x + 6.0) {
                dirX = 1.0;
            }
        } else {
            e.estado = EstadoAgente::Patrulla;
            dirX = static_cast<double>(e.dirPatrulla);
        }

        e.vx = dirX * velEnemigo;

        // Sub-pasos horizontales
        const double despX = e.vx * dt;
        const int pasosX = qMax(1, static_cast<int>(qCeil(qAbs(despX) / 2.0)));
        const double pasoX = despX / pasosX;
        const double limiteDer = static_cast<double>(qMax(0, m_escenario.ancho() - e.ancho));

        bool chocoPared = false;
        for (int i = 0; i < pasosX; ++i) {
            const double candidatoX = qBound(0.0, e.x + pasoX, limiteDer);
            const int xCheck = (pasoX > 0.0)
                                   ? static_cast<int>(candidatoX + e.ancho)
                                   : static_cast<int>(candidatoX);

            const int yTop = static_cast<int>(e.y + 4);
            const int yMid = static_cast<int>(e.y + e.alto * 0.5);
            const int yBot = static_cast<int>(e.y + e.alto - 4);

            const bool golpeaPared = (pasoX != 0.0) && (
                                         m_escenario.esPared(xCheck, yTop) ||
                                         m_escenario.esPared(xCheck, yMid) ||
                                         m_escenario.esPared(xCheck, yBot)
                                         );

            if (golpeaPared) {
                e.vx = 0.0;
                chocoPared = true;
                break;
            }
            e.x = candidatoX;
        }

        if (e.estado == EstadoAgente::Patrulla && (chocoPared || e.x <= 15.0 || e.x >= limiteDer - 15.0)) {
            e.dirPatrulla *= -1;
        }

        // Gravedad y sub-pasos verticales
        e.vy = qMin(maxVelocidadCaida, e.vy + gravedad * dt);
        const double despY = e.vy * dt;
        const int pasosY = qMax(1, static_cast<int>(qCeil(qAbs(despY) / 2.0)));
        const double pasoY = despY / pasosY;
        e.enSuelo = false;

        for (int i = 0; i < pasosY; ++i) {
            const double candidatoY = e.y + pasoY;
            if (pasoY >= 0.0) {
                const int pieY = static_cast<int>(candidatoY + e.alto);
                const int xPieIzq = static_cast<int>(e.x + 6);
                const int xPieCentro = static_cast<int>(e.x + e.ancho * 0.5);
                const int xPieDer = static_cast<int>(e.x + e.ancho - 6);

                const bool tocaSuelo =
                    m_escenario.esSuelo(xPieIzq, pieY) ||
                    m_escenario.esSuelo(xPieCentro, pieY) ||
                    m_escenario.esSuelo(xPieDer, pieY) ||
                    m_escenario.esPared(xPieCentro, pieY);

                if (tocaSuelo) {
                    e.y = static_cast<double>(pieY - e.alto);
                    e.vy = 0.0;
                    e.enSuelo = true;
                    break;
                }
            }
            e.y = candidatoY;
        }

        if (e.y > m_escenario.alto()) {
            spawnEnemigo(idx, static_cast<double>(m_escenario.ancho() - 120));
            continue;
        }

        // Animacion (mas lenta si esta ralentizado)
        const bool dirAnterior = e.mirandoIzquierda;
        if (dirX < -0.01) {
            e.mirandoIzquierda = true;
        } else if (dirX > 0.01) {
            e.mirandoIzquierda = false;
        }
        const bool cambioDir = (e.mirandoIzquierda != dirAnterior);
        const double intervaloAnim = (e.tiempoRalentizado > 0.0) ? 0.22 : 0.11;

        if (m_spriteEnemigo) {
            e.tiempoAnim += dt;
            if (e.tiempoAnim >= intervaloAnim || cambioDir) {
                if (e.tiempoAnim >= intervaloAnim) {
                    e.tiempoAnim = 0.0;
                }
                e.frame = m_spriteEnemigo->obtenerCuadroAnimado(0, e.mirandoIzquierda);
            }
        }

        // Empuje fisico a Spider-Man -> pierde 1 vida
        const QRectF rectEnemigo(e.x, e.y, e.ancho, e.alto);
        if (rectSpider.intersects(rectEnemigo) && m_invencibilidadSpider <= 0.0) {
            m_vidasSpider = qMax(0, m_vidasSpider - 1);
            m_invencibilidadSpider = 1.2;

            // Sentido del empuje segun de que lado venga el enemigo
            const double empDir = (m_spiderX < e.x) ? -1.0 : 1.0;
            aplicarEmpujeSpider(empDir * 340.0, -270.0);

            if (m_vidasSpider <= 0) {
                m_estado = EstadoNivel::Derrota;
                return;
            }
        }
        // Dano al Palacio si el enemigo llega a la zona izquierda
        if (e.x <= 120.0 && m_cooldownDanoPalacio <= 0.0) {
            m_cooldownDanoPalacio = 1.5;
            m_integridadPalacio = qMax(0, m_integridadPalacio - 10);
        }
    }
}

void EscenaNivel2::actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana)
{
    Q_UNUSED(altoVentana);

    if (teclasActivas.contains(Qt::Key_J) ||
        teclasActivas.contains(Qt::Key_E) ||
        teclasActivas.contains(Qt::Key_Control)) {
        dispararTelarana();
    }

    avanzarSpiderYColisiones(dt, teclasActivas);
    actualizarCamara(anchoMundoVisible); // Vista fija: m_camaraX = 0
    actualizarTelaranas(dt);
    actualizarEnemigos(dt);

    if (m_estado != EstadoNivel::Jugando) {
        return;
    }

    if (m_integridadPalacio <= 0 || m_vidasSpider <= 0) {
        m_estado = EstadoNivel::Derrota;
        return;
    }

    m_tiempoRestante -= dt;
    if (m_tiempoRestante <= 0.0) {
        m_tiempoRestante = 0.0;
        m_puntosGanados += 1000;
        m_estado = EstadoNivel::Victoria;
    }
}

void EscenaNivel2::dibujarTelaranas(QPainter &painter, double escala, int offsetX) const
{
    for (const TelaranaProyectil &w : m_telaranas) {
        if (!w.activa) {
            continue;
        }

        const int screenX = offsetX + static_cast<int>((w.x - m_camaraX) * escala);
        const int screenY = static_cast<int>(w.y * escala);
        const int screenW = qMax(16, static_cast<int>(w.ancho * escala));
        const int screenH = qMax(16, static_cast<int>(w.alto * escala));

        if (m_tieneSpriteTelaranaValido && !m_spriteTelarana.isNull()) {
            painter.drawPixmap(QRect(screenX, screenY, screenW, screenH), m_spriteTelarana);
        } else {
            painter.save();
            painter.setPen(QPen(Qt::white, 2));
            const int cx = screenX + screenW / 2;
            const int cy = screenY + screenH / 2;
            const int r = qMin(screenW, screenH) / 2;
            painter.drawEllipse(QPoint(cx, cy), r, r);
            painter.drawLine(cx - r, cy, cx + r, cy);
            painter.drawLine(cx, cy - r, cx, cy + r);
            painter.restore();
        }
    }
}

void EscenaNivel2::dibujarEnemigos(QPainter &painter, double escala, int offsetX, int altoVentana) const
{
    Q_UNUSED(altoVentana);

    for (const EnemigoAgente &e : m_enemigos) {
        if (!e.vivo) {
            continue;
        }

        const int screenX = offsetX + static_cast<int>((e.x - m_camaraX) * escala);
        const int screenY = static_cast<int>(e.y * escala);
        const int screenW = qMax(18, static_cast<int>(e.ancho * escala));
        const int screenH = qMax(26, static_cast<int>(e.alto * escala));
        const QRect destino(screenX, screenY, screenW, screenH);

        if (!e.frame.isNull()) {
            painter.drawPixmap(destino, e.frame);
            if (e.tiempoRalentizado > 0.0) {
                painter.save();
                painter.fillRect(destino, QColor(80, 190, 255, 95));
                painter.restore();
            }
        } else {
            painter.setBrush(e.tiempoRalentizado > 0.0 ? QColor(80, 170, 240) : QColor(40, 40, 60));
            painter.drawRect(destino);
        }
    }
}

void EscenaNivel2::dibujar(QPainter &painter, double escala, int offsetX, int anchoMundoVisible, int altoVentana)
{
    if (m_invencibilidadSpider > 0.0 && static_cast<int>(m_invencibilidadSpider * 10.0) % 2 == 0) {
        QPixmap respaldo = m_frameActualSpider;
        m_frameActualSpider = QPixmap();
        EscenaBase::dibujar(painter, escala, offsetX, anchoMundoVisible, altoVentana);
        m_frameActualSpider = respaldo;
    } else {
        EscenaBase::dibujar(painter, escala, offsetX, anchoMundoVisible, altoVentana);
    }

    dibujarEnemigos(painter, escala, offsetX, altoVentana);
    dibujarTelaranas(painter, escala, offsetX);
}

void EscenaNivel2::dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const
{
    Q_UNUSED(altoVentana);
    painter.save();

    int activos = 0;
    for (const EnemigoAgente &e : m_enemigos) {
        if (e.vivo) ++activos;
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(20, 28, QString("%1  |  Enemigos: %2  |  2 disparos = eliminar").arg(nombreNivel()).arg(activos));
    painter.drawText(anchoVentana - 520, 28,
                     QString("Vidas: %1  |  Puntos: %2  |  Palacio: %3%  |  Tiempo: %4s")
                         .arg(m_vidasSpider)
                         .arg(puntuacionTotal)
                         .arg(m_integridadPalacio)
                         .arg(static_cast<int>(m_tiempoRestante)));

    painter.restore();
}
