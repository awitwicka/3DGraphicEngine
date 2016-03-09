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

    //(a11x^2 + a22y^2 + 2a12xy + 2a14x + 2a24y + a44)+
    //+2(a13x + a23y + a34)z + a33z^2 = 0
    QVector4D row1 = Dm.row(0);
    QVector4D row2 = Dm.row(1);
    QVector4D row3 = Dm.row(2);
    QVector4D row4 = Dm.row(3);
    float a = row3.z();
    float b = 2*(row1.z()*x + row2.z()*y + row3.w());
    float c = row1.x()*pow(x,2) + row2.y()*pow(y,2) + 2*row1.y()*x*y + 2*row1.w()*x + 2*row2.w()*y + row4.w();
    float delta = pow(b,2) - (4*a*c);
    if (delta<0)
        return -1;
    float z1 = (-b + sqrt(delta)) / (2*a);
    float z2 = (-b - sqrt(delta)) / (2*a);
    if (z1 >= z2)
        return z1;
    else
        return z2;

    /*
    float zz = -((x*x)/(a*a) + (y*y)/(b*b) -1)*(c*c);
    if (zz < 0)
        return -1;
    else
        return zz;//return sqrt(zz);
    */
}

QVector4D Elipse::fd(float x, float y, float z, QMatrix4x4 m)
{
    QMatrix4x4 D = QMatrix4x4(a,0,0,0,
                              0,b,0,0,
                              0,0,c,0,
                              0,0,0,-1.0);
    QMatrix4x4 Dm = m.inverted().transposed()*D*m.inverted();
    QVector4D row1 = Dm.row(0);
    QVector4D row2 = Dm.row(1);
    QVector4D row3 = Dm.row(2);
    QVector4D row4 = Dm.row(3);

   return 2*QVector4D(x*row1.x()+y*row1.y()+z*row1.z()+row1.w(),
                    x*row2.x()+y*row2.y()+z*row2.z()+row2.w(),
                    x*row3.x()+y*row3.y()+z*row3.z()+row3.w(),
                    x*row4.x()+y*row4.y()+z*row4.z()+row4.w());
}


