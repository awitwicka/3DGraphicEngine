#ifndef TORUS_H
#define TORUS_H

#include "cadobject.h"
#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>

class Torus : public CADObject
{
    float Usegments;
    float Vsegments;
    float R;
    float r;
    void InitializeTorus();
    void Clear();

public:
    Torus();
    //~Torus() {};
    QVector<QVector4D> points;
    //QVector<int> indices;
    QVector<QPoint> indices;
    QMatrix4x4 modelMatrix;
    //float ulen,vlen;
    QVector4D f(float u, float v);
    //void Draw(QPainter &painter, bool isStereo);

    void setU(float h);
    void setV(float l);
    void setR(float R);
    void setr(float r);
};

#endif // TORUS_H
