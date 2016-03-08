#ifndef ELIPSE_H
#define ELIPSE_H

#include <QVector4D>
#include <QMatrix4x4>

class Elipse
{
    float a;
    float b;
    float c;
    void InitializeElipse();
public:
    Elipse();
    float  f(float x, float y, QMatrix4x4 m);
};

#endif // ELIPSE_H
