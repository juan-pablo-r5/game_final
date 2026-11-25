#include "escenario.h"

#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

bool Escenario::cargarNivel(const QString& rutaVisual, const QString& rutaColision) {
    QImage nuevoVisual;
    QImage nuevaColision;

    if (!nuevoVisual.load(rutaVisual)) {
        qWarning() << "No se pudo cargar mapa visual:" << rutaVisual;
        return false;
    }

    if (!nuevaColision.load(rutaColision)) {
        qWarning() << "No se pudo cargar mapa de colisiones:" << rutaColision;
        return false;
    }

    nuevaColision = nuevaColision.convertToFormat(QImage::Format_ARGB32);

    if (nuevoVisual.width() != nuevaColision.width() ||
        nuevoVisual.height() != nuevaColision.height()) {
        qWarning() << "Las dimensiones del mapa visual y de colisiones no coinciden";
        return false;
    }

    if (nuevoVisual.isNull() || nuevaColision.isNull()) {
        return false;
    }

    m_visual = nuevoVisual;
    m_colision = nuevaColision;
    return true;
}

bool Escenario::cargarNivelPorNumero(int numeroNivel) {
    if (numeroNivel <= 0) {
        qWarning() << "Numero de nivel invalido:" << numeroNivel;
        return false;
    }

    const QString subrutaVisual = QString("assets/nivel%1/visual.png").arg(numeroNivel);
    const QString subrutaColision = QString("assets/nivel%1/colisiones.png").arg(numeroNivel);

    const QString baseDir = QCoreApplication::applicationDirPath();
    QString rutaVisual = QDir(baseDir).filePath(subrutaVisual);
    QString rutaColision = QDir(baseDir).filePath(subrutaColision);

    if (!QFile::exists(rutaVisual) || !QFile::exists(rutaColision)) {
        rutaVisual = subrutaVisual;
        rutaColision = subrutaColision;
    }

    if (!cargarNivel(rutaVisual, rutaColision)) {
        return false;
    }

    m_nivelActual = numeroNivel;
    qDebug() << "Nivel" << m_nivelActual << "cargado (" << ancho() << "x" << alto() << ")";
    return true;
}

int Escenario::nivelActual() const {
    return m_nivelActual;
}

int Escenario::ancho() const {
    return m_colision.width();
}

int Escenario::alto() const {
    return m_colision.height();
}

const QImage& Escenario::mapaVisual() const {
    return m_visual;
}

const QImage& Escenario::mapaColision() const {
    return m_colision;
}

bool Escenario::esSuelo(int x, int y) const {
    if (!dentroDeLimites(x, y)) {
        return false;
    }

    const QColor c = m_colision.pixelColor(x, y);
    return c.alpha() > 0 && c.green() > 200 && c.red() < 50 && c.blue() < 50;
}

bool Escenario::esPared(int x, int y) const {
    if (!dentroDeLimites(x, y)) {
        return true;
    }

    const QColor c = m_colision.pixelColor(x, y);
    return c.alpha() > 0 && c.red() > 200 && c.green() < 50 && c.blue() < 50;
}

bool Escenario::dentroDeLimites(int x, int y) const {
    return x >= 0 && x < m_colision.width() && y >= 0 && y < m_colision.height();
}