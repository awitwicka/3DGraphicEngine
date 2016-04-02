#ifndef CADOBJECT_H
#define CADOBJECT_H

#include <QMatrix4x4>
#include <QPainter>
#include <QVector>
#include "constants.h"

class CADObject{
    public:
        CADObject();
        virtual ~CADObject()=0;
        QVector<QVector4D> points;
        QVector<QPoint> indices;
        QMatrix4x4 modelMatrix;
        //float ulen,vlen;
        //virtual QVector4D f(float u, float v);
        virtual void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

        QColor LRedColor;
        QColor RBlueColor;
        QColor Color;
};

/*class Torus:public CADObject{
    //...
    QVector4D f(float u, float v);
};
class BezierPatch:public CADObject{
    //...
    QVector4D f(float u, float v);
};*/

#endif // CADOBJECT_H
