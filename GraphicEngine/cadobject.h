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
        //float ulen,vlen;
        //virtual QVector4D f(float u, float v);
        virtual void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

        QColor LRedColor;
        QColor RBlueColor;
        QColor Color;

        //getters setters
        virtual void setPoints(const QVector<QVector4D> &value);
        virtual void setIndices(const QVector<QPoint> &value);
        virtual QVector<QVector4D> getPoints() const;
        virtual QVector<QPoint> getIndices() const;

private:
        QMatrix4x4 modelMatrix;
        QVector<QVector4D> points;
        QVector<QPoint> indices;
};

#endif // CADOBJECT_H
