#include "escenanivel1.h"
#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QRectF>
#include <QTransform>
#include <QtMath>
#include <stdexcept>
#include <QImage>

void EscenaNivel1::inicializar()
{
    m_estado = EstadoNivel::Jugando;
    m_puntosGanados = 0;
    m_tiempoRestante = 180.0;
    m_ciudadanosSalvados = 0;
    m_ciudadanos.clear();

    if (!m_escenario.cargarNivelPorNumero(1)) {
        throw std::runtime_error("No se pudo cargar el Nivel 1.");
    }

    posicionarSpiderEnInicio();
    cargarHojasCiudadanos();
    inicializarCiudadanos();
}

QPixmap EscenaNivel1::cargarPixmapAsset(const QString &subruta) const
{
    QPixmap pix;
    const QString baseDir = QCoreApplication::applicationDirPath();
    const QStringList candidatos = {
        QDir(baseDir).filePath(subruta),
        QDir(baseDir).filePath("../" + subruta),
        QDir(baseDir).filePath("../../" + subruta),
        QDir(baseDir).filePath("../../../" + subruta),
        QDir(baseDir).filePath("../../../../" + subruta),
        QDir::current().filePath(subruta)
    };

    for (const QString &ruta : candidatos) {
        const QString limpia = QDir::cleanPath(ruta);
        if (QFile::exists(limpia) && pix.load(limpia)) {
            return pix;
        }
    }

    return QPixmap();
}

void EscenaNivel1::cargarHojasCiudadanos()
{
    m_hojasTipos.clear();
    m_cuadrosPorTipo.clear();

    const QStringList archivos = {
        "assets/nivel1/ciudadano1.png",
        "assets/nivel1/ciudadano2.png",
        "assets/nivel1/ciudadano3.png"
    };
    const QVector<int> cuadrosSugeridos = {9, 7, 7};

    for (int i = 0; i < archivos.size(); ++i) {
        QPixmap hoja = cargarPixmapAsset(archivos[i]);
        if (hoja.isNull()) {
            qWarning() << "No se pudo cargar sprite de ciudadano:" << archivos[i];
        }
        m_hojasTipos.append(hoja);
        m_cuadrosPorTipo.append(cuadrosSugeridos[i]);
    }
}

static QVector<QRect> obtenerCuadrosDeTipo(int tipoIdx, const QPixmap &hoja)
{
    Q_UNUSED(tipoIdx);
    QVector<QRect> rects;
    if (hoja.isNull() || hoja.width() <= 0 || hoja.height() <= 0) {
        return rects;
    }

    const QImage img = hoja.toImage().convertToFormat(QImage::Format_ARGB32);
    const int w = img.width();
    const int h = img.height();

    QVector<bool> columnaOcupada(w, false);
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (qAlpha(img.pixel(x, y)) > 15) {
                columnaOcupada[x] = true;
                break;
            }
        }
    }

    int x = 0;
    while (x < w) {
        while (x < w && !columnaOcupada[x]) {
            ++x;
        }
        if (x >= w) {
            break;
        }

        const int inicio = x;
        while (x < w && columnaOcupada[x]) {
            ++x;
        }
        const int fin = x; // Exclusivo

        const int anchoSprite = fin - inicio;
        // Ignora motas diminutas de 1-2 px sueltos y toma el sprite completo
        if (anchoSprite >= 4) {
            const int x0 = qMax(0, inicio - 1);
            const int x1 = qMin(w, fin + 1);
            rects.append(QRect(x0, 0, x1 - x0, h));
        }
    }

    // Respaldo por si la imagen no tuviera canal alfa transparente
    if (rects.isEmpty()) {
        const int n = 6;
        const int wBase = qMax(1, w / n);
        for (int k = 0; k < n; ++k) {
            const int x0 = k * wBase;
            const int wReal = (k == n - 1) ? (w - x0) : wBase;
            if (wReal > 2) {
                rects.append(QRect(x0, 0, wReal, h));
            }
        }
    }

    return rects;
}


void EscenaNivel1::configurarCuadrosDesdeHoja(Ciudadano &c, int tipoIdx, int cuadrosEstimados) const
{
    Q_UNUSED(cuadrosEstimados);

    c.cuadrosCaminar = obtenerCuadrosDeTipo(tipoIdx, c.hoja);
    c.indiceCuadro = 0;
    c.tiempoAnim = 0.0;

    const double altoObjetivo = (tipoIdx == 0) ? 52.0 : 68.0;

    if (!c.cuadrosCaminar.isEmpty()) {
        const QRect &r0 = c.cuadrosCaminar.first();
        const int hBase = qMax(1, r0.height());
        const int wBase = qMax(1, r0.width());
        const double factorEscala = altoObjetivo / static_cast<double>(hBase);

        c.alto = qMax(24, static_cast<int>(qRound(hBase * factorEscala)));
        c.ancho = qMax(16, static_cast<int>(qRound(wBase * factorEscala)));
    } else if (!c.hoja.isNull() && c.hoja.height() > 0 && c.hoja.width() > 0) {
        const double factorEscala = altoObjetivo / static_cast<double>(c.hoja.height());
        c.alto = qMax(24, static_cast<int>(qRound(c.hoja.height() * factorEscala)));
        c.ancho = qMax(16, static_cast<int>(qRound(c.hoja.width() * factorEscala)));
    }
}

void EscenaNivel1::ubicarCiudadanoEnSuelo(Ciudadano &c) const
{
    if (m_escenario.mapaColision().isNull()) {
        return;
    }

    const int xCheck = qBound(0, static_cast<int>(c.x + c.ancho * 0.5), m_escenario.ancho() - 1);
    for (int y = 20; y < m_escenario.alto(); ++y) {
        if (m_escenario.esSuelo(xCheck, y)) {
            c.y = static_cast<double>(y - c.alto);
            return;
        }
    }
}

void EscenaNivel1::inicializarCiudadanos()
{
    m_ciudadanos.clear();

    struct PuntoSuelo {
        double propX;        // Posición horizontal relativa al mapa (0.0 .. 1.0)
        int tipoIdx;         // 0: ciudadano1, 1: ciudadano2, 2: ciudadano3
        bool caminando;      // true en zonas con más espacio libre en el suelo
        bool mirarIzq;       // Dirección inicial
        double rangoPatrulla; // Mitad del rango horizontal de patrulla (px)
    };

    const QVector<PuntoSuelo> puntos = {
        { 0.045, 0, true,  false, 28.0 },
        { 0.095, 1, true,  true,  22.0 },
        { 0.195, 2, true,  false, 26.0 },
        { 0.245, 0, true,  true,  22.0 },
        { 0.382, 0, false, false,  0.0 },
        { 0.418, 1, false, false,  0.0 },
        { 0.575, 2, true,  false, 32.0 },
        { 0.625, 0, true,  true,  32.0 },
        { 0.592, 2, false, false,  0.0 },
        { 0.618, 0, false, true,   0.0 },
        { 0.840, 1, true,  false, 30.0 },
        { 0.980, 2, true,  true,  26.0 }
    };

    const double anchoMapa = static_cast<double>(qMax(600, m_escenario.ancho()));
    const int altoEsc = m_escenario.alto();
    const int anchoEsc = m_escenario.ancho();
    const bool hayCol = !m_escenario.mapaColision().isNull();

    for (int i = 0; i < puntos.size(); ++i) {
        const PuntoSuelo &p = puntos[i];
        Ciudadano c;

        const int tipo = p.tipoIdx % qMax(1, m_hojasTipos.size());
        c.hoja = m_hojasTipos.value(tipo);
        c.caminando = p.caminando;
        c.vx = c.caminando ? (36.0 + (i % 3) * 4.0) : 0.0;
        c.mirandoIzquierda = p.mirarIzq;
        c.salvado = false;

        configurarCuadrosDesdeHoja(c, tipo, m_cuadrosPorTipo.value(tipo, 1));

        c.x = qBound(50.0, anchoMapa * p.propX, anchoMapa - c.ancho - 50.0);
        c.y = static_cast<double>(qMax(20, altoEsc - c.alto - 20));

        if (hayCol && anchoEsc > 0 && altoEsc > 0) {
            bool encontrado = false;
            const QVector<int> offsets = {0, -10, 10, -20, 20};
            for (int dx : offsets) {
                const int xPrueba = qBound(2, static_cast<int>(c.x + c.ancho * 0.5) + dx, anchoEsc - 3);
                // Recorre de abajo hacia arriba para tomar la superficie del suelo real
                for (int y = altoEsc - 2; y >= 10; --y) {
                    if (m_escenario.esSuelo(xPrueba, y)) {
                        const int yArriba = qMax(0, y - 6);
                        if (!m_escenario.esPared(xPrueba, yArriba) && !m_escenario.esSuelo(xPrueba, yArriba)) {
                            c.x = qBound(50.0, static_cast<double>(xPrueba - c.ancho * 0.5), anchoMapa - c.ancho - 50.0);
                            c.y = static_cast<double>(y - c.alto);
                            encontrado = true;
                            break;
                        }
                    }
                }
                if (encontrado) {
                    break;
                }
            }
        }

        c.xMin = qMax(50.0, c.x - p.rangoPatrulla);
        c.xMax = qMin(anchoMapa - c.ancho - 50.0, c.x + p.rangoPatrulla);

        m_ciudadanos.append(c);
    }
}

void EscenaNivel1::actualizarCiudadanos(double dt)
{
    const bool hayColision = !m_escenario.mapaColision().isNull();
    const int anchoEsc = m_escenario.ancho();
    const int altoEsc = m_escenario.alto();

    for (Ciudadano &c : m_ciudadanos) {
        if (c.salvado) {
            continue;
        }

        if (c.caminando) {
            const double xAnterior = c.x;
            const double dir = c.mirandoIzquierda ? -1.0 : 1.0;
            double siguienteX = c.x + dir * c.vx * dt;

            if (siguienteX <= c.xMin) {
                siguienteX = c.xMin;
                c.mirandoIzquierda = false;
            } else if (siguienteX >= c.xMax) {
                siguienteX = c.xMax;
                c.mirandoIzquierda = true;
            }

            bool puedeAvanzar = true;
            if (hayColision && anchoEsc > 0 && altoEsc > 0) {
                const int xBorde = qBound(0,
                                          (dir > 0.0) ? static_cast<int>(siguienteX + c.ancho)
                                                      : static_cast<int>(siguienteX),
                                          anchoEsc - 1);
                const int xCentroSig = qBound(0, static_cast<int>(siguienteX + c.ancho * 0.5), anchoEsc - 1);
                const int yPie = static_cast<int>(c.y + c.alto);
                const int yCentro = qBound(0, static_cast<int>(c.y + c.alto * 0.5), altoEsc - 1);

                const bool tocaPared = m_escenario.esPared(xBorde, yCentro)
                                       || m_escenario.esPared(xBorde, qMax(0, yPie - 8));

                int ySueloSig = -1;
                for (int dy = -2; dy <= 6; ++dy) {
                    const int yCheck = yPie + dy;
                    if (yCheck >= 0 && yCheck < altoEsc && m_escenario.esSuelo(xCentroSig, yCheck)) {
                        ySueloSig = yCheck;
                        break;
                    }
                }

                if (tocaPared || ySueloSig < 0) {
                    puedeAvanzar = false;
                    c.mirandoIzquierda = !c.mirandoIzquierda;
                    c.x = xAnterior;
                } else {
                    c.x = siguienteX;
                    c.y = static_cast<double>(ySueloSig - c.alto);
                }
            } else {
                c.x = siguienteX;
            }

            if (c.cuadrosCaminar.size() > 1) {
                c.tiempoAnim += dt;
                if (c.tiempoAnim >= 0.09) {
                    c.tiempoAnim = 0.0;
                    if (c.indiceCuadro < 1 || c.indiceCuadro >= c.cuadrosCaminar.size() - 1) {
                        c.indiceCuadro = 1;
                    } else {
                        c.indiceCuadro += 1;
                    }
                }
            } else {
                c.indiceCuadro = 0;
            }
        } else {
            c.indiceCuadro = 0;
            c.tiempoAnim = 0.0;
        }

        const QRectF hitSpider(m_spiderX - 4.0, m_spiderY - 4.0, m_anchoSpider + 8.0, m_altoSpider + 8.0);
        const QRectF hitCiudadano(c.x, c.y, c.ancho, c.alto);
        if (hitSpider.intersects(hitCiudadano)) {
            c.salvado = true;
            m_ciudadanosSalvados += 1;
            m_puntosGanados += 100;
        }
    }
}

void EscenaNivel1::actualizar(double dt, const QSet<int> &teclasActivas, int anchoMundoVisible, int altoVentana)
{
    Q_UNUSED(altoVentana);

    avanzarSpiderYColisiones(dt, teclasActivas);
    actualizarCamara(anchoMundoVisible);

    if (m_estado != EstadoNivel::Jugando) {
        return;
    }

    actualizarCiudadanos(dt);

    if (!m_ciudadanos.isEmpty() && m_ciudadanosSalvados >= m_ciudadanos.size()) {
        m_puntosGanados += 500;
        m_estado = EstadoNivel::Victoria;
        return;
    }

    m_tiempoRestante -= dt;
    if (m_tiempoRestante <= 0.0) {
        m_tiempoRestante = 0.0;
        m_estado = EstadoNivel::Derrota;
    }
}

void EscenaNivel1::dibujarCiudadanos(QPainter &painter, double escala, int offsetX) const
{
    for (const Ciudadano &c : m_ciudadanos) {
        if (c.salvado) {
            continue;
        }

        const int screenX = offsetX + static_cast<int>((c.x - m_camaraX) * escala);
        const int screenY = static_cast<int>(c.y * escala);
        const int screenW = qMax(16, static_cast<int>(c.ancho * escala));
        const int screenH = qMax(24, static_cast<int>(c.alto * escala));
        const QRect destino(screenX, screenY, screenW, screenH);

        if (!c.hoja.isNull() && !c.cuadrosCaminar.isEmpty()) {
            const int idx = qBound(0, c.indiceCuadro, c.cuadrosCaminar.size() - 1);
            const QRect recorte = c.cuadrosCaminar[idx];
            QPixmap cuadro = c.hoja.copy(recorte);
            if (c.mirandoIzquierda) {
                cuadro = cuadro.transformed(QTransform().scale(-1, 1));
            }
            painter.save();
            painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
            painter.drawPixmap(destino, cuadro);
            painter.restore();
        }
    }
}

void EscenaNivel1::dibujar(QPainter &painter, double escala, int offsetX, int anchoMundoVisible, int altoVentana)
{
    EscenaBase::dibujar(painter, escala, offsetX, anchoMundoVisible, altoVentana);
    dibujarCiudadanos(painter, escala, offsetX);
}

void EscenaNivel1::dibujarHUD(QPainter &painter, int anchoVentana, int altoVentana, int puntuacionTotal) const
{
    Q_UNUSED(altoVentana);
    painter.save();

    const int totalSeg = qMax(0, static_cast<int>(m_tiempoRestante));
    const int minutos = totalSeg / 60;
    const int segundos = totalSeg % 60;

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(20, 28, nombreNivel());
    painter.drawText(anchoVentana - 540, 28,
                     QString("Salvados: %1/%2  |  Puntos: %3  |  Tiempo: %4:%5")
                         .arg(m_ciudadanosSalvados)
                         .arg(m_ciudadanos.size())
                         .arg(puntuacionTotal)
                         .arg(minutos, 2, 10, QChar('0'))
                         .arg(segundos, 2, 10, QChar('0')));

    painter.restore();
}