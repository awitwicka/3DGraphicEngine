#ifndef BEZIER_H
#define BEZIER_H

#include "cadobject.h"
#include "marker.h"
#include "segment.h"
#include "cadmarkerobject.h"

class Bezier : public CADObject, public CADMarkerObject
{
    static int id;
    QVector4D getBezierPoint(Segment seg, float t);

    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;
    void Clear();
public:
    void InitializeSpline(QMatrix4x4 matrix);
    Bezier();
    QList<Segment> BezierSegments;
    Bezier(QList<Marker*> const & m, QMatrix4x4 matrix);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker *> getMarkers() const;
};

/*
 *
 * void func(QList<int> list) // func will use a copy of list
void func(QList<int>& list) // func will use the same instance of list
void func(QList<int> const & list) // func will use the same instance of list and will not* modify it
void func(QList<int>* list) // func will take a pointer to a QList<int> (that may be valid or not...)
 * */

#endif // BEZIER_H
