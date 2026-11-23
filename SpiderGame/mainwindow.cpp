#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    resize(1280, 800);

    if (!cambiarEscena(&m_nivel1)) {
        qWarning() << "No se pudo inicializar el Nivel 1";
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::cambiarEscena(Escena *nuevaEscena) {
    if (!nuevaEscena) {
        return false;
    }

    if (!nuevaEscena->cargar()) {
        qWarning() << "Error al cargar escena:" << nuevaEscena->nombre();
        return false;
    }

    m_escenaActual = nuevaEscena;
    qDebug() << "Escena activa:" << m_escenaActual->nombre();
    update();
    return true;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    if (m_escenaActual) {
        m_escenaActual->dibujar(painter, width(), height());
    } else {
        painter.fillRect(rect(), Qt::black);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!event) {
        return;
    }

    if (event->key() == Qt::Key_1) {
        cambiarEscena(&m_nivel1);
        return;
    }

    if (event->key() == Qt::Key_2) {
        cambiarEscena(&m_nivel2);
        return;
    }

    if (m_escenaActual) {
        m_escenaActual->manejarTecla(event, width());
        update();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (m_escenaActual) {
        m_escenaActual->manejarClic(event);
    } else {
        QMainWindow::mousePressEvent(event);
    }
}