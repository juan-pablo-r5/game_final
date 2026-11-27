#include "spiderman.h"
#include <QBrush>
#include <QDebug>
#include <QPen>
#include <QTransform>
#include <stdexcept>

SpiderMan::SpiderMan(QGraphicsItem *parent)
    : QObject(nullptr)
    , QGraphicsPixmapItem(parent)
    , EntidadBase(150.0, 500.0, 1.5, 100)
    , m_spriteSheet(nullptr)
    , m_hitbox(nullptr)
    , m_timerAnimacion(new QTimer(this))
    , m_timerEstado(new QTimer(this))
    , m_sonidoAccion(new QMediaPlayer(this))
    , m_salidaAudio(new QAudioOutput(this))
    , m_estado(EstadoTemperamento::Normal)
    , m_mirandoDerecha(true)
    , m_enSuelo(false)
    , m_coyoteFrames(0)
    , m_velocidadBase(4.5)
    , m_gravedadBase(0.65)
    , m_impulsoSaltoBase(-14.5)
{
    m_sonidoAccion->setAudioOutput(m_salidaAudio);
    m_sonidoAccion->setSource(QUrl("qrc:/sounds/ataque.mp3"));
    m_salidaAudio->setVolume(0.7);

    try {
        m_spriteSheet = new SpriteSheet(":/sprites/spider.jpg", 2);
        // Ajusta ancho/alto del cuadro segun la fila elegida del sprite sheet
        m_spriteSheet->configurarTamanoCuadro(32, 40);
        m_spriteSheet->agregarAnimacion(QRect(0, 0, 128, 40), 4);   // Animacion 0: stand/idle
        m_spriteSheet->agregarAnimacion(QRect(0, 48, 256, 40), 8);  // Animacion 1: running
    } catch (const std::exception &ex) {
        qWarning() << "Excepcion en SpiderMan:" << ex.what();
        delete m_spriteSheet;
        m_spriteSheet = nullptr;
    }

    m_hitbox = new QGraphicsRectItem(10, 10, 44, 60, this);
    m_hitbox->setBrush(Qt::NoBrush);
    m_hitbox->setPen(Qt::NoPen);

    m_timerEstado->setSingleShot(true);
    connect(m_timerEstado, &QTimer::timeout, this, &SpiderMan::expirarEstadoEspecial);

    connect(m_timerAnimacion, &QTimer::timeout, this, &SpiderMan::avanzarAnimacion);
    m_timerAnimacion->start(90);

    sincronizarPosicionGrafica();
    avanzarAnimacion();
}

SpiderMan::~SpiderMan()
{
    delete m_spriteSheet;
    m_spriteSheet = nullptr;
}

void SpiderMan::actualizarLogica(double dt)
{
    if (!m_activa) return;

    double gravedadEfectiva = m_gravedadBase;
    if (m_estado == EstadoTemperamento::PesadoEmpapado) {
        gravedadEfectiva *= 1.45;
    }

    m_x += m_vx * dt;

    if (!m_enSuelo) {
        m_vy += gravedadEfectiva * dt;
        m_y += m_vy * dt;
    }

    actualizarFisicaPlataformas();
    sincronizarPosicionGrafica();
}

void SpiderMan::actualizarFisicaPlataformas()
{
    bool tocoSuelo = false;
    QList<QGraphicsItem *> itemsColision = m_hitbox->collidingItems();

    for (QGraphicsItem *item : itemsColision) {
        QString tipo = item->data(0).toString();
        if (tipo == "plataforma" || tipo == "suelo") {
            qreal techo = item->sceneBoundingRect().top();
            qreal baseHitbox = m_hitbox->sceneBoundingRect().bottom();

            if (m_vy >= 0.0 && baseHitbox >= techo - 6.0) {
                qreal nuevaY = techo - (m_hitbox->rect().top() + m_hitbox->rect().height());
                m_y = nuevaY;
                m_vy = 0.0;
                tocoSuelo = true;
                break;
            }
        }
    }

    if (tocoSuelo) {
        m_enSuelo = true;
        m_coyoteFrames = MAX_COYOTE_FRAMES;
    } else {
        m_enSuelo = false;
        if (m_coyoteFrames > 0) {
            --m_coyoteFrames;
        }
    }
}

void SpiderMan::recibirDano(int cantidad)
{
    if (!m_activa || cantidad <= 0) return;

    EntidadBase::recibirDano(cantidad);
    if (!m_activa) {
        emit jugadorMurio();
    }
}

void SpiderMan::moverIzquierda()
{
    m_mirandoDerecha = false;
    double factorVel = (m_estado == EstadoTemperamento::EnojonSobrecargado) ? 1.4 : 1.0;
    m_vx = -m_velocidadBase * factorVel;
}

void SpiderMan::moverDerecha()
{
    m_mirandoDerecha = true;
    double factorVel = (m_estado == EstadoTemperamento::EnojonSobrecargado) ? 1.4 : 1.0;
    m_vx = m_velocidadBase * factorVel;
}

void SpiderMan::detenerHorizontal()
{
    m_vx = 0.0;
}

void SpiderMan::saltar()
{
    if (m_coyoteFrames <= 0) return;

    double impulso = m_impulsoSaltoBase;
    if (m_estado == EstadoTemperamento::PesadoEmpapado) {
        // Menor tension de salto/red: salto mas corto y pesado
        impulso *= 0.72;
    }

    m_vy = impulso;
    m_enSuelo = false;
    m_coyoteFrames = 0;
    sincronizarPosicionGrafica();
}

double SpiderMan::calcularImpulsoDisparoRed(double alcanceBase, double &anguloDesviacionGrados) const
{
    double alcanceFinal = alcanceBase;
    anguloDesviacionGrados = 0.0;

    if (m_estado == EstadoTemperamento::EnojonSobrecargado) {
        alcanceFinal *= 2.0; // Alcance duplicado
        // Leve desviacion aleatoria entre -9 y +9 grados por sobrecarga/estruendo
        anguloDesviacionGrados = (QRandomGenerator::global()->bounded(19) - 9) * 1.0;
    } else if (m_estado == EstadoTemperamento::PesadoEmpapado) {
        alcanceFinal *= 0.75; // Menor tension de red
    }

    return alcanceFinal;
}

void SpiderMan::reproducirSonidoRed()
{
    if (m_sonidoAccion->playbackState() == QMediaPlayer::PlayingState) {
        m_sonidoAccion->stop();
    }
    m_sonidoAccion->play();
}

void SpiderMan::activarEnojonSobrecargado()
{
    m_estado = EstadoTemperamento::EnojonSobrecargado;
    m_timerEstado->start(8000); // 8 segundos
    emit estadoCambiado(m_estado);
}

void SpiderMan::activarPesadoEmpapado()
{
    m_estado = EstadoTemperamento::PesadoEmpapado;
    m_timerEstado->start(12000); // 12 segundos
    emit estadoCambiado(m_estado);
}

void SpiderMan::expirarEstadoEspecial()
{
    m_estado = EstadoTemperamento::Normal;
    emit estadoCambiado(m_estado);
}

QRectF SpiderMan::getHitboxEscena() const
{
    return m_hitbox->sceneBoundingRect();
}

void SpiderMan::sincronizarPosicionGrafica()
{
    setPos(m_x, m_y);
}

void SpiderMan::avanzarAnimacion()
{
    if (!m_spriteSheet) return;

    unsigned int anim = (qAbs(m_vx) > 0.1) ? 1u : 0u;
    QPixmap cuadro = m_spriteSheet->obtenerCuadroAnimado(anim);
    if (cuadro.isNull()) return;

    cuadro = limpiarFondoVerde(cuadro);
    if (!m_mirandoDerecha) {
        cuadro = cuadro.transformed(QTransform().scale(-1, 1));
    }
    setPixmap(cuadro);
}

QPixmap SpiderMan::limpiarFondoVerde(const QPixmap &cuadro) const
{
    QImage img = cuadro.toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor c = img.pixelColor(x, y);
            // Elimina verde chroma key intenso del sprite sheet
            if (c.green() > 170 && c.red() < 110 && c.blue() < 110) {
                img.setPixelColor(x, y, Qt::transparent);
            }
        }
    }
    return QPixmap::fromImage(img);
}
