#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "escenabase.h"
#include "escenanivel1.h"
#include "escenanivel2.h"
#include "spritesheet.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
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

private slots:
    void actualizarJuego();

private:
    void inicializarSpriteSpider();
    void cargarNivel(int numeroNivel);
    void calcularTransformacionCamara(double &escala, int &anchoMundoVisible, int &offsetX) const;

    Ui::MainWindow *ui;
    QTimer m_timer;
    QSet<int> m_teclasActivas;

    EscenaBase *m_escenaActual = nullptr;
    SpriteSheet *m_spriteSpider = nullptr;

    int m_nivelActual = 1;
    int m_puntuacion = 0;
};

#endif // MAINWINDOW_H
