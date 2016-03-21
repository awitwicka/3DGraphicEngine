#ifndef TORUS_H
#define TORUS_H

#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>

class Torus
{
    float Usegments;
    float Vsegments;
    float R;
    float r;
    void InitializeTorus();
    void Clear();

public:
    Torus();

    QVector<QVector4D> points;
    //QVector<int> indices;
    QVector<QPoint> indices;
    QMatrix4x4 modelMatrix;
    //float ulen,vlen;
    QVector4D f(float u, float v);

    void setU(float h);
    void setV(float l);
    void setR(float R);
    void setr(float r);
};

#endif // TORUS_H
