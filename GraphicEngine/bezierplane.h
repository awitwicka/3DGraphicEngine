#ifndef BEZIERPLANE_H
#define BEZIERPLANE_H

#include "cadobject.h"
#include "bicubicsegment.h"
#include "cadsplinepatch.h"

class BezierPlane : public CADObject, public CADSplinePatch
{
    static int id;
    //QVector4D getBezierPoint(BicubicSegment seg, float t);
    
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    void InitializeMarkers();
    void Clear();
public:
    int X;
    int Y;
    float Width;
    float Height;
    int U;
    int V;
    
    BezierPlane();
    BezierPlane(QMatrix4x4 matrix);
    QList<BicubicSegment> BezierSegments;
    QVector<QList<Marker*>> BezierSegMarkers;
    //QList<Marker*[]> BezierSegMarkers;

    void InitializeSpline(QMatrix4x4 matrix);
    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    //void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker> getMarkers();
};

#endif // BEZIERPLANE_H
