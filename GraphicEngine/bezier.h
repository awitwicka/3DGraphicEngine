#ifndef BEZIER_H
#define BEZIER_H

#include "cadobject.h"
#include "marker.h"


class Bezier : public CADObject
{
    void InitializeBezier();
    QVector4D getBezierPoint(Marker m1, Marker m2, Marker m3, Marker m4, float t);
    void getCurveGeometry();
public:
    Bezier();
    Bezier(Marker*m1, Marker*m2, Marker*m3, Marker*m4);
    QList<Marker*> markers;
    QVector<QVector4D> points;
    QVector<QPoint> indices;
};

#endif // BEZIER_H
