#ifndef BEZIERPLANE_H
#define BEZIERPLANE_H

#include "cadobject.h"
#include "cadmarkerobject.h"
#include "bicubicsegment.h"

class BezierPlane : public CADObject, public CADMarkerObject
{
    static int id;
    //QVector4D getBezierPoint(BicubicSegment seg, float t);

    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;
    void Clear();
public:
    BezierPlane();
    BezierPlane(QList<Marker*> const & m, QMatrix4x4 matrix);
    QList<BicubicSegment> BezierSegments;

    void InitializeSpline(QMatrix4x4 matrix);
    //void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker *> getMarkers() const;
};

#endif // BEZIERPLANE_H
