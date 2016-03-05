#include "torus.h"

Torus::Torus() : Usegments(3), Vsegments(3), R(100), r(30)
{
    InitializeTorus();
}

void Torus::InitializeTorus()
{
    Clear();
    const float ustep = 1/Usegments;
    const float vstep = 1/Vsegments;
    //points
    for (int v = 0; v < Vsegments; v++) {
        for (int u = 0; u < Usegments; u++) {
            points.append(f(ustep*u,vstep*v));
        }
    }
    //indices along u
    for (int v = 0; v < Vsegments; v++) {
        for (int u = 0; u < Usegments-1; u++) {
            indices.append(QPoint((v*Usegments)+u,
                                  (v*Usegments)+(u+1)));
        }
    }
    //indices along v
    for (int v = 0; v < Vsegments-1; v++) {
        for (int u = 0; u < Usegments; u++) {
            indices.append(QPoint((v*Usegments)+u,
                                  ((v+1)*Usegments)+u));
        }
    }
    //last row and column
    for (int v = 0; v < Vsegments; v++)
        indices.append(QPoint((v*Usegments),
                              (v*Usegments)+Usegments-1));
    for (int u = 0; u < Usegments; u++)
        indices.append(QPoint(u,
                              ((Vsegments-1)*Usegments+u)));
}

void Torus::Clear()
{
    indices.clear();
    points.clear();
}

//u and v are between 0  1
QVector4D Torus::f(float u, float v)
{
    const float pi = asin(1.0)*2.0;
    u *= 2*pi;
    v *= 2*pi;
    float x = (R+(r*cos(u)))*cos(v);
    float y = (R+(r*cos(u)))*sin(v);
    float z = r*sin(u);
    return QVector4D(x, y, z, 1);
}

void Torus::setU(float u)
{
    Usegments = u;
    InitializeTorus();
}

void Torus::setV(float v)
{
    Vsegments = v;
    InitializeTorus();
}

void Torus::setR(float R)
{
    this->R = R;
    InitializeTorus();
}

void Torus::setr(float r)
{
    this->r = r;
    InitializeTorus();
}

