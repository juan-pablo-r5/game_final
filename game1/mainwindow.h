#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "escenabase.h"
#include "escenanivel1.h"
#include "escenanivel2.h"
#include "spritesheet.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPixmap>
#include <QRect>
#include <QSet>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void actualizarJuego();

private:
    enum class PantallaJuego {
        MenuInicio,
        SeleccionNivel,
        Jugando,
        Victoria,
        Derrota
    };

    void inicializarSpriteSpider();
    void cargarRecursosUI();
    QPixmap cargarPixmapFlexible(const QString &rutaRecurso, const QString &subrutaAsset) const;

    void iniciarNivel(int numeroNivel);
    void cargarNivel(int numeroNivel);
    void calcularTransformacionCamara(double &escala, int &anchoMundoVisible, int &offsetX) const;

    // Pantalla de inicio
    void dibujarPantallaInicio(QPainter &painter);
    QRect rectBotonInicio(int indice) const;
    void ejecutarOpcionInicio(int indice);

    // Pantalla de seleccion de nivel
    void dibujarSeleccionNivel(QPainter &painter);
    QRect rectTarjetaNivel(int indice) const;
    QRect rectBotonVolverSeleccion() const;
    void confirmarSeleccionNivel(int numeroNivel);

    Ui::MainWindow *ui;
    QTimer m_timer;
    QSet<int> m_teclasActivas;

    EscenaBase *m_escenaActual = nullptr;
    SpriteSheet *m_spriteSpider = nullptr;

    PantallaJuego m_pantalla = PantallaJuego::MenuInicio;
    QPixmap m_fondoInicio;
    QPixmap m_fondoSeleccionNivel;

    int m_opcionInicioSeleccionada = 0; // 0: Jugar, 1: Seleccionar Nivel, 2: Salir
    int m_nivelSeleccionado = 1;        // 1 o 2 (0 si esta resaltado "Volver al inicio")

    int m_nivelActual = 1;
    int m_puntuacion = 0;
};

#endif // MAINWINDOW_H