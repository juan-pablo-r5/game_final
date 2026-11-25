#include "entidadbase.h"

EntidadBase::EntidadBase(double x, double y, double masa, int vida)
    : m_x(x)
    , m_y(y)
    , m_vx(0.0)
    , m_vy(0.0)
    , m_masa(masa > 0.0 ? masa : 1.0)
    , m_vida(vida)
    , m_activa(true)
{
}

void EntidadBase::recibirDano(int cantidad)
{
    if (cantidad <= 0 || !m_activa) return;
    m_vida -= cantidad;
    if (m_vida <= 0) {
        m_vida = 0;
        m_activa = false;
    }
}

void EntidadBase::setPosicion(double x, double y)
{
    m_x = x;
    m_y = y;
}

void EntidadBase::setVelocidad(double vx, double vy)
{
    m_vx = vx;
    m_vy = vy;
}
