#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QRect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    resize(1280, 800);

    if (!m_escenario.cargarNivelPorNumero(1)) {
        qWarning() << "No se pudo cargar el Nivel 1 al iniciar";
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    const QImage &visual = m_escenario.mapaVisual();
    if (visual.isNull()) {
        return;
    }

    const int maxCamaraX = qMax(0, visual.width() - width());
    m_camaraX = qBound(0, m_camaraX, maxCamaraX);

    const int anchoVisible = qMin(width(), visual.width() - m_camaraX);
    const int altoVisible = qMin(height(), visual.height());

    const QRect fuente(m_camaraX, 0, anchoVisible, altoVisible);
    const QRect destino(0, 0, anchoVisible, altoVisible);

    painter.drawImage(destino, visual, fuente);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_1) {
        if (m_escenario.cargarNivelPorNumero(1)) {
            m_camaraX = 0;
            update();
        }
        return;
    }

    if (event->key() == Qt::Key_2) {
        if (m_escenario.cargarNivelPorNumero(2)) {
            m_camaraX = 0;
            update();
        }
        return;
    }

    const int pasoCamara = 40;
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        m_camaraX += pasoCamara;
        update();
    } else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        m_camaraX -= pasoCamara;
        update();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || m_escenario.mapaColision().isNull()) {
        return;
    }

    const int mundoX = event->pos().x() + m_camaraX;
    const int mundoY = event->pos().y();

    const bool suelo = m_escenario.esSuelo(mundoX, mundoY);
    const bool pared = m_escenario.esPared(mundoX, mundoY);
    const QColor c = m_escenario.mapaColision().pixelColor(mundoX, mundoY);

    qDebug() << "Nivel:" << m_escenario.nivelActual()
             << "| Pos mundo (" << mundoX << "," << mundoY << ")"
             << "| RGBA(" << c.red() << "," << c.green() << "," << c.blue() << "," << c.alpha() << ")"
             << "| esSuelo:" << suelo
             << "| esPared:" << pared;
}