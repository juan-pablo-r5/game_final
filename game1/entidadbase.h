#ifndef ENTIDADBASE_H
#define ENTIDADBASE_H

class EntidadBase
{
public:
    EntidadBase(double x = 0.0, double y = 0.0, double masa = 1.0, int vida = 100);
    virtual ~EntidadBase() = default;

    // Interfaz polimórfica propia
    virtual void actualizarLogica(double dt) = 0;
    virtual void recibirDano(int cantidad);

    // Getters y setters
    double getX() const { return m_x; }
    double getY() const { return m_y; }
    double getVx() const { return m_vx; }
    double getVy() const { return m_vy; }
    double getMasa() const { return m_masa; }
    int getVida() const { return m_vida; }
    bool estaActiva() const { return m_activa; }

    void setPosicion(double x, double y);
    void setVelocidad(double vx, double vy);
    void setActiva(bool activa) { m_activa = activa; }

protected:
    double m_x;
    double m_y;
    double m_vx;
    double m_vy;
    double m_masa;
    int m_vida;
    bool m_activa;
};

#endif // ENTIDADBASE_H
