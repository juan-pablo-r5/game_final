#pragma once

#include "escenario.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMainWindow>
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
    Ui::MainWindow *ui;
    Escenario m_escenario;
    int m_camaraX = 0;
}; // MAINWINDOW_H
