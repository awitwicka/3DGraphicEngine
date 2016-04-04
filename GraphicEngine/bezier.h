#ifndef BEZIER_H
#define BEZIER_H

#include "cadobject.h"
#include "marker.h"
#include "segment.h"

class Bezier : public CADObject
{
    void InitializeBezier();
    QVector4D getBezierPoint(Segment seg, float t);
    void getCurveGeometry();

    QVector<QVector4D> points;
    QVector<QPoint> indices;
public:
    Bezier();
    QList<Segment> Segments;
    Bezier(QList<Marker*> markers);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
};

#endif // BEZIER_H
