#ifndef SPIDERMAN_H
#define SPIDERMAN_H

#include "entidadbase.h"
#include "spritesheet.h"
#include <QAudioOutput>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QMediaPlayer>
#include <QObject>
#include <QRandomGenerator>
#include <QTimer>

enum class EstadoTemperamento {
    Normal,
    EnojonSobrecargado, // +40% vel, x2 alcance red, leve desviacion por 8s
    PesadoEmpapado     // +gravedad, menor salto/red, inmune al fuego por 12s
};

class SpiderMan : public QObject, public QGraphicsPixmapItem, public EntidadBase
{
    Q_OBJECT

public:
    explicit SpiderMan(QGraphicsItem *parent = nullptr);
    ~SpiderMan() override;

    // Implementacion de EntidadBase (herencia propia)
    void actualizarLogica(double dt) override;
    void recibirDano(int cantidad) override;

    // Controles y acciones
    void moverIzquierda();
    void moverDerecha();
    void detenerHorizontal();
    void saltar();
    double calcularImpulsoDisparoRed(double alcanceBase, double &anguloDesviacionGrados) const;
    void reproducirSonidoRed();

    // Estados especiales
    void activarEnojonSobrecargado();
    void activarPesadoEmpapado();
    EstadoTemperamento getEstadoTemperamento() const { return m_estado; }
    bool esInmuneAlFuego() const { return m_estado == EstadoTemperamento::PesadoEmpapado; }

    // Colisiones y camara
    QRectF getHitboxEscena() const;
    void actualizarFisicaPlataformas();

signals:
    void estadoCambiado(EstadoTemperamento nuevoEstado);
    void jugadorMurio();

private slots:
    void avanzarAnimacion();
    void expirarEstadoEspecial();

private:
    SpriteSheet *m_spriteSheet;
    QGraphicsRectItem *m_hitbox;
    QTimer *m_timerAnimacion;
    QTimer *m_timerEstado;

    QMediaPlayer *m_sonidoAccion;
    QAudioOutput *m_salidaAudio;

    EstadoTemperamento m_estado;
    bool m_mirandoDerecha;
    bool m_enSuelo;
    int m_coyoteFrames;
    static constexpr int MAX_COYOTE_FRAMES = 6;

    double m_velocidadBase;
    double m_gravedadBase;
    double m_impulsoSaltoBase;

    void sincronizarPosicionGrafica();
    QPixmap limpiarFondoVerde(const QPixmap &cuadro) const;
};

#endif // SPIDERMAN_H
