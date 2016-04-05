#ifndef BEZIER_H
#define BEZIER_H

#include "cadobject.h"
#include "marker.h"
#include "segment.h"

class Bezier : public CADObject
{
    static int id;
    void InitializeBezier();
    QVector4D getBezierPoint(Segment seg, float t);
    void getCurveGeometry();

    QVector<QVector4D> points;
    QVector<QPoint> indices;
public:
    QString name;
    QString idname;
    Bezier();
    QList<Segment> Segments;
    QList<Marker*> markers;
    Bezier(QList<Marker*> const & m);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
};

/*
 *
 * void func(QList<int> list) // func will use a copy of list
void func(QList<int>& list) // func will use the same instance of list
void func(QList<int> const & list) // func will use the same instance of list and will not* modify it
void func(QList<int>* list) // func will take a pointer to a QList<int> (that may be valid or not...)
 * */

#endif // BEZIER_H
