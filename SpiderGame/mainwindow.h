#pragma once

#include "escena.h"
#include "escenanivel1.h"
#include "escenanivel2.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool cambiarEscena(Escena *nuevaEscena);

    Ui::MainWindow *ui;
    EscenaNivel1 m_nivel1;
    EscenaNivel2 m_nivel2;
    Escena *m_escenaActual = nullptr;
};