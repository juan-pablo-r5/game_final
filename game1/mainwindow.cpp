#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1280, 800);

    inicializarSpriteSpider();

    // Nivel 1 por defecto
    m_puntuacion = 0;
    cargarNivel(1);

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    m_timer.start(16); // ~60 FPS
}

MainWindow::~MainWindow()
{
    delete m_escenaActual;
    m_escenaActual = nullptr;

    delete m_spriteSpider;
    m_spriteSpider = nullptr;

    delete ui;
}

void MainWindow::inicializarSpriteSpider()
{
    try {
        delete m_spriteSpider;
        m_spriteSpider = new SpriteSheet(":/sprites/spider.jpg", 2);
        m_spriteSpider->eliminarColorFondo();
        m_spriteSpider->configurarTamanoCuadro(24, 34);
        m_spriteSpider->agregarAnimacion(QRect(4, 14, 60, 36), 2, 25);
        m_spriteSpider->agregarAnimacion(QRect(4, 58, 240, 36), 8, 27);
    } catch (const std::exception &e) {
        qWarning() << "Error cargando sprite de Spider-Man:" << e.what();
        delete m_spriteSpider;
        m_spriteSpider = nullptr;
    }
}

void MainWindow::cargarNivel(int numeroNivel)
{
    delete m_escenaActual;
    m_escenaActual = nullptr;

    m_nivelActual = (numeroNivel == 2) ? 2 : 1;

    try {
        if (m_nivelActual == 2) {
            m_escenaActual = new EscenaNivel2();
        } else {
            m_escenaActual = new EscenaNivel1();
        }

        m_escenaActual->inicializar();
        if (m_spriteSpider) {
            m_escenaActual->asignarSpriteSpider(m_spriteSpider);
        }
    } catch (const std::exception &e) {
        qWarning() << "Error cargando nivel" << m_nivelActual << ":" << e.what();
        delete m_escenaActual;
        m_escenaActual = nullptr;
    }
}

void MainWindow::calcularTransformacionCamara(double &escala, int &anchoMundoVisible, int &offsetX) const
{
    if (!m_escenaActual) {
        escala = 1.0;
        anchoMundoVisible = width();
        offsetX = 0;
        return;
    }

    const QImage &visual = m_escenaActual->escenario().mapaVisual();
    if (visual.isNull() || visual.height() <= 0) {
        escala = 1.0;
        anchoMundoVisible = width();
        offsetX = 0;
        return;
    }

    escala = static_cast<double>(height()) / visual.height();
    anchoMundoVisible = qMin(visual.width(), static_cast<int>(width() / escala));
    const int anchoDestino = static_cast<int>(anchoMundoVisible * escala);
    offsetX = (width() - anchoDestino) / 2;
}

void MainWindow::actualizarJuego()
{
    if (!m_escenaActual) {
        return;
    }

    const double dt = 0.016;
    double escala = 1.0;
    int anchoMundoVisible = width();
    int offsetX = 0;
    calcularTransformacionCamara(escala, anchoMundoVisible, offsetX);

    m_escenaActual->actualizar(dt, m_teclasActivas, anchoMundoVisible, height());

    if (m_escenaActual->estado() == EstadoNivel::Victoria) {
        m_puntuacion += m_escenaActual->puntosGanados();
        if (m_nivelActual == 1) {
            cargarNivel(2); // Borra Nivel 1 y crea Nivel 2
        } else {
            // Si tienes stackedWidget en el .ui:
            // ui->stackedWidget->setCurrentWidget(ui->pageFin);
        }
    } else if (m_escenaActual->estado() == EstadoNivel::Derrota) {
        // Si tienes stackedWidget en el .ui:
        // ui->stackedWidget->setCurrentWidget(ui->pageFin);
    }

    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (!m_escenaActual) {
        return;
    }

    double escala = 1.0;
    int anchoMundoVisible = width();
    int offsetX = 0;
    calcularTransformacionCamara(escala, anchoMundoVisible, offsetX);

    m_escenaActual->dibujar(painter, escala, offsetX, anchoMundoVisible, height());
    m_escenaActual->dibujarHUD(painter, width(), height(), m_puntuacion);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    if (event->key() == Qt::Key_1) {
        cargarNivel(1);
        update();
        return;
    }

    if (event->key() == Qt::Key_2) {
        cargarNivel(2);
        update();
        return;
    }

    m_teclasActivas.insert(event->key());
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    m_teclasActivas.remove(event->key());
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_escenaActual) {
        return;
    }

    const Escenario &esc = m_escenaActual->escenario();
    if (esc.mapaColision().isNull()) {
        return;
    }

    double escala = 1.0;
    int anchoMundoVisible = width();
    int offsetX = 0;
    calcularTransformacionCamara(escala, anchoMundoVisible, offsetX);

    const int mundoX = static_cast<int>((event->pos().x() - offsetX) / escala) + m_escenaActual->camaraX();
    const int mundoY = static_cast<int>(event->pos().y() / escala);

    const bool suelo = esc.esSuelo(mundoX, mundoY);
    const bool pared = esc.esPared(mundoX, mundoY);
    const QColor c = esc.mapaColision().pixelColor(mundoX, mundoY);

    qDebug() << "Nivel:" << m_nivelActual
             << "| Pos mundo (" << mundoX << "," << mundoY << ")"
             << "| RGBA(" << c.red() << "," << c.green() << "," << c.blue() << "," << c.alpha() << ")"
             << "| esSuelo:" << suelo
             << "| esPared:" << pared;
}
