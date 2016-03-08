#include "elipse.h"

Elipse::Elipse():  a(50), b(100), c(50)
{
    InitializeElipse();
}

void Elipse::InitializeElipse()
{

}

float Elipse::f(float x, float y, QMatrix4x4 m)
{
    QMatrix4x4 D = QMatrix4x4(a,0,0,0,
                              0,b,0,0,
                              0,0,c,0,
                              0,0,0,-1);
    QMatrix4x4 Dm = m.inverted().transposed()*D*m.inverted();

    //QVector4D u;
    float zz = -((x*x)/(a*a) + (y*y)/(b*b) -1)*(c*c);
    if (zz < 0)
        return -1;
    else
        return zz;//return sqrt(zz);

    //posc linie jak podstw x, y sprawdz czy = z
}


