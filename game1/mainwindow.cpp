#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QLinearGradient>
#include <QPainter>
#include <QPen>
#include <QStringList>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1280, 800);
    setWindowTitle("Spider-Man: Bogota en Llamas");
    setMouseTracking(true);
    if (ui->centralwidget) {
        ui->centralwidget->setMouseTracking(true);
    }

    inicializarSpriteSpider();
    cargarRecursosUI();

    m_puntuacion = 0;
    m_pantalla = PantallaJuego::MenuInicio;

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

QPixmap MainWindow::cargarPixmapFlexible(const QString &rutaRecurso, const QString &subrutaAsset) const
{
    QPixmap pix;
    if (!rutaRecurso.isEmpty() && pix.load(rutaRecurso)) {
        return pix;
    }

    const QString baseDir = QCoreApplication::applicationDirPath();
    const QStringList candidatos = {
        QDir(baseDir).filePath(subrutaAsset),
        QDir(baseDir).filePath("../" + subrutaAsset),
        QDir(baseDir).filePath("../../" + subrutaAsset),
        QDir(baseDir).filePath("../../../" + subrutaAsset),
        QDir(baseDir).filePath("../../../../" + subrutaAsset),
        QDir::current().filePath(subrutaAsset)
    };

    for (const QString &ruta : candidatos) {
        const QString limpia = QDir::cleanPath(ruta);
        if (QFile::exists(limpia) && pix.load(limpia)) {
            return pix;
        }
    }

    return QPixmap();
}

void MainWindow::cargarRecursosUI()
{
    m_fondoInicio = cargarPixmapFlexible(":/sprites/BogotaEnLlamas.png", "assets/BogotaEnLlamas.png");
    if (m_fondoInicio.isNull()) {
        m_fondoInicio = cargarPixmapFlexible(":/sprites/BogotaEnLlamas.jpg", "assets/BogotaEnLlamas.jpg");
    }
    if (m_fondoInicio.isNull()) {
        qWarning() << "No se encontro BogotaEnLlamas(.png/.jpg) en recursos ni en assets/";
    }

    m_fondoSeleccionNivel = cargarPixmapFlexible(":/sprites/seleccion_nivel.png", "assets/seleccion_nivel.png");
    if (m_fondoSeleccionNivel.isNull()) {
        m_fondoSeleccionNivel = cargarPixmapFlexible(":/sprites/seleccion_nivel.jpg", "assets/seleccion_nivel.jpg");
    }
    if (m_fondoSeleccionNivel.isNull()) {
        qWarning() << "No se encontro seleccion_nivel(.png/.jpg) en recursos ni en assets/";
    }
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

void MainWindow::iniciarNivel(int numeroNivel)
{
    m_teclasActivas.clear();
    cargarNivel(numeroNivel);
    if (m_escenaActual) {
        m_pantalla = PantallaJuego::Jugando;
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

QRect MainWindow::rectBotonInicio(int indice) const
{
    const int anchoBtn = 340;
    const int altoBtn = 54;
    const int espacio = 16;
    const int x = (width() - anchoBtn) / 2;
    const int yBase = height() - 235;
    return QRect(x, yBase + indice * (altoBtn + espacio), anchoBtn, altoBtn);
}

void MainWindow::ejecutarOpcionInicio(int indice)
{
    if (indice == 0) {
        m_puntuacion = 0;
        iniciarNivel(1);
    } else if (indice == 1) {
        m_nivelSeleccionado = 1;
        m_pantalla = PantallaJuego::SeleccionNivel;
    } else if (indice == 2) {
        QApplication::quit();
    }
    update();
}

void MainWindow::dibujarPantallaInicio(QPainter &painter)
{
    painter.fillRect(rect(), QColor(12, 16, 26));

    if (!m_fondoInicio.isNull()) {
        const QPixmap escalada = m_fondoInicio.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, escalada);

        // Viñeta superior e inferior para que el título y los botones resalten nítidos
        QLinearGradient sombraSup(0, 0, 0, 220);
        sombraSup.setColorAt(0.0, QColor(0, 0, 0, 215));
        sombraSup.setColorAt(1.0, QColor(0, 0, 0, 0));
        painter.fillRect(0, 0, width(), 220, sombraSup);

        QLinearGradient sombraInf(0, height() - 280, 0, height());
        sombraInf.setColorAt(0.0, QColor(0, 0, 0, 0));
        sombraInf.setColorAt(0.55, QColor(0, 0, 0, 175));
        sombraInf.setColorAt(1.0, QColor(0, 0, 0, 235));
        painter.fillRect(0, height() - 280, width(), 280, sombraInf);
    }

    // Subtítulo superior y título principal estilo arcade
    painter.setPen(QColor(255, 220, 110));
    painter.setFont(QFont("Courier New", 14, QFont::Bold));
    painter.drawText(QRect(0, 34, width(), 28), Qt::AlignCenter, "SPIDER-MAN  -  9 DE ABRIL DE 1948");

    const QRect rTitulo(0, 66, width(), 64);
    painter.setPen(QColor(0, 0, 0, 230));
    painter.setFont(QFont("Courier New", 42, QFont::Bold));
    painter.drawText(rTitulo.translated(4, 4), Qt::AlignCenter, "BOGOTA EN LLAMAS");
    painter.setPen(QColor(245, 238, 220));
    painter.drawText(rTitulo, Qt::AlignCenter, "BOGOTA EN LLAMAS");

    const QStringList opciones = {
        "JUGAR",
        "SELECCIONAR NIVEL",
        "SALIR"
    };

    for (int i = 0; i < opciones.size(); ++i) {
        const QRect r = rectBotonInicio(i);
        const bool seleccionado = (i == m_opcionInicioSeleccionada);

        painter.fillRect(r.translated(4, 4), QColor(0, 0, 0, 210));

        if (seleccionado) {
            painter.fillRect(r, QColor(195, 32, 38, 240));
            painter.setPen(QPen(QColor(255, 230, 130), 3));
        } else {
            painter.fillRect(r, QColor(24, 30, 48, 225));
            painter.setPen(QPen(QColor(180, 185, 200), 2));
        }
        painter.drawRect(r);

        painter.setPen(seleccionado ? QColor(255, 245, 210) : QColor(225, 225, 235));
        painter.setFont(QFont("Courier New", 16, QFont::Bold));
        const QString texto = seleccionado ? ("> " + opciones[i] + " <") : opciones[i];
        painter.drawText(r, Qt::AlignCenter, texto);
    }

    painter.setPen(QColor(190, 195, 210));
    painter.setFont(QFont("Courier New", 11));
    painter.drawText(QRect(0, height() - 34, width(), 24),
                     Qt::AlignCenter,
                     "Usa Flechas / Mouse para elegir  |  ENTER o Click para confirmar");
}

QRect MainWindow::rectTarjetaNivel(int indice) const
{
    const int anchoTarjeta = 380;
    const int altoTarjeta = 82;
    const int separacion = 20;
    const int x = width() - anchoTarjeta - 170;
    const int yBase = 230;

    return QRect(x, yBase + indice * (altoTarjeta + separacion), anchoTarjeta, altoTarjeta);
}

QRect MainWindow::rectBotonVolverSeleccion() const
{
    const int anchoBtn = 380;
    const int altoBtn = 52;
    const int x = width() - anchoBtn - 170;
    const int y = height() - 120;

    return QRect(x, y, anchoBtn, altoBtn);
}

void MainWindow::confirmarSeleccionNivel(int numeroNivel)
{
    m_nivelSeleccionado = (numeroNivel == 2) ? 2 : 1;
    m_puntuacion = 0;
    iniciarNivel(m_nivelSeleccionado);
    update();
}

void MainWindow::dibujarSeleccionNivel(QPainter &painter)
{
    painter.fillRect(rect(), QColor(12, 16, 26));

    if (!m_fondoSeleccionNivel.isNull()) {
        const QPixmap escalada = m_fondoSeleccionNivel.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, escalada);
    }

    QLinearGradient velo(width() * 0.35, 0, width(), 0);
    velo.setColorAt(0.0, QColor(0, 0, 0, 15));
    velo.setColorAt(0.45, QColor(8, 12, 22, 145));
    velo.setColorAt(1.0, QColor(8, 12, 22, 200));
    painter.fillRect(rect(), velo);

    const int xCol = width() - 550; // Alineado con los botones más a la izquierda
    const int wCol = 380;

    painter.setPen(QColor(255, 220, 110));
    painter.setFont(QFont("Courier New", 13, QFont::Bold));
    painter.drawText(QRect(xCol, 145, wCol, 26), Qt::AlignCenter, "MISIONES DISPONIBLES");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier New", 26, QFont::Bold));
    painter.drawText(QRect(xCol, 175, wCol, 42), Qt::AlignCenter, "SELECCIONAR NIVEL");

    const QStringList etiquetas = {
        "NIVEL 1",
        "NIVEL 2"
    };

    for (int i = 0; i < etiquetas.size(); ++i) {
        const QRect r = rectTarjetaNivel(i);
        const bool activa = (m_nivelSeleccionado == (i + 1));

        painter.fillRect(r.translated(4, 4), QColor(0, 0, 0, 210));
        painter.fillRect(r, activa ? QColor(195, 32, 38, 235) : QColor(20, 26, 42, 215));
        painter.setPen(QPen(activa ? QColor(255, 230, 130) : QColor(175, 182, 200), activa ? 3 : 2));
        painter.drawRect(r);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Courier New", 16, QFont::Bold));
        painter.drawText(r, Qt::AlignCenter, etiquetas[i]);
    }

    const QRect rVolver = rectBotonVolverSeleccion();
    const bool hoverVolver = (m_nivelSeleccionado == 0);
    painter.fillRect(rVolver.translated(4, 4), QColor(0, 0, 0, 210));
    painter.fillRect(rVolver, hoverVolver ? QColor(195, 32, 38, 235) : QColor(20, 26, 42, 215));
    painter.setPen(QPen(hoverVolver ? QColor(255, 230, 130) : QColor(175, 182, 200), 2));
    painter.drawRect(rVolver);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier New", 13, QFont::Bold));
    painter.drawText(rVolver, Qt::AlignCenter, "VOLVER AL INICIO");
}

void MainWindow::actualizarJuego()
{
    if (m_pantalla != PantallaJuego::Jugando || !m_escenaActual) {
        update();
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
        m_pantalla = PantallaJuego::Victoria;
    } else if (m_escenaActual->estado() == EstadoNivel::Derrota) {
        m_pantalla = PantallaJuego::Derrota;
    }

    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (m_pantalla == PantallaJuego::MenuInicio) {
        dibujarPantallaInicio(painter);
        return;
    }

    if (m_pantalla == PantallaJuego::SeleccionNivel) {
        dibujarSeleccionNivel(painter);
        return;
    }

    if (m_pantalla == PantallaJuego::Jugando && m_escenaActual) {
        double escala = 1.0;
        int anchoMundoVisible = width();
        int offsetX = 0;
        calcularTransformacionCamara(escala, anchoMundoVisible, offsetX);

        m_escenaActual->dibujar(painter, escala, offsetX, anchoMundoVisible, height());
        m_escenaActual->dibujarHUD(painter, width(), height(), m_puntuacion);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    if (m_pantalla == PantallaJuego::MenuInicio) {
        if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
            m_opcionInicioSeleccionada = (m_opcionInicioSeleccionada + 2) % 3;
            update();
        } else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {
            m_opcionInicioSeleccionada = (m_opcionInicioSeleccionada + 1) % 3;
            update();
        } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            ejecutarOpcionInicio(m_opcionInicioSeleccionada);
        }
        return;
    }

    if (m_pantalla == PantallaJuego::SeleccionNivel) {
        if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
            m_nivelSeleccionado = (m_nivelSeleccionado == 2) ? 1 : ((m_nivelSeleccionado == 1) ? 0 : 2);
            update();
        } else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {
            m_nivelSeleccionado = (m_nivelSeleccionado == 0) ? 1 : ((m_nivelSeleccionado == 1) ? 2 : 0);
            update();
        } else if (event->key() == Qt::Key_1) {
            confirmarSeleccionNivel(1);
        } else if (event->key() == Qt::Key_2) {
            confirmarSeleccionNivel(2);
        } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            if (m_nivelSeleccionado == 0) {
                m_pantalla = PantallaJuego::MenuInicio;
                update();
            } else {
                confirmarSeleccionNivel(m_nivelSeleccionado);
            }
        } else if (event->key() == Qt::Key_Escape) {
            m_pantalla = PantallaJuego::MenuInicio;
            update();
        }
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        m_teclasActivas.clear();
        m_pantalla = PantallaJuego::MenuInicio;
        update();
        return;
    }

    if (event->key() == Qt::Key_1) {
        iniciarNivel(1);
        update();
        return;
    }

    if (event->key() == Qt::Key_2) {
        iniciarNivel(2);
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

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pantalla == PantallaJuego::MenuInicio) {
        for (int i = 0; i < 3; ++i) {
            if (rectBotonInicio(i).contains(event->pos())) {
                if (m_opcionInicioSeleccionada != i) {
                    m_opcionInicioSeleccionada = i;
                    update();
                }
                break;
            }
        }
    } else if (m_pantalla == PantallaJuego::SeleccionNivel) {
        int nuevaSeleccion = -1;
        if (rectTarjetaNivel(0).contains(event->pos())) {
            nuevaSeleccion = 1;
        } else if (rectTarjetaNivel(1).contains(event->pos())) {
            nuevaSeleccion = 2;
        } else if (rectBotonVolverSeleccion().contains(event->pos())) {
            nuevaSeleccion = 0;
        }
        if (nuevaSeleccion != -1 && nuevaSeleccion != m_nivelSeleccionado) {
            m_nivelSeleccionado = nuevaSeleccion;
            update();
        }
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (m_pantalla == PantallaJuego::MenuInicio) {
        for (int i = 0; i < 3; ++i) {
            if (rectBotonInicio(i).contains(event->pos())) {
                ejecutarOpcionInicio(i);
                return;
            }
        }
        return;
    }

    if (m_pantalla == PantallaJuego::SeleccionNivel) {
        if (rectTarjetaNivel(0).contains(event->pos())) {
            confirmarSeleccionNivel(1);
        } else if (rectTarjetaNivel(1).contains(event->pos())) {
            confirmarSeleccionNivel(2);
        } else if (rectBotonVolverSeleccion().contains(event->pos())) {
            m_pantalla = PantallaJuego::MenuInicio;
            update();
        }
        return;
    }

    if (m_pantalla != PantallaJuego::Jugando || !m_escenaActual) {
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