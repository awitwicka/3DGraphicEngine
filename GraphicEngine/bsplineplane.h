#ifndef BSPLINEPLANE_H
#define BSPLINEPLANE_H

#include "cadobject.h"
#include "bicubicsegment.h"
#include "cadsplinepatch.h"

class BSplinePlane : public CADObject, public CADSplinePatch
{
    static int id;
    //QVector4D getBezierPoint(BicubicSegment seg, float t);

    //QVector<QVector4D> pointsCurve;
    //QVector<QPoint> indicesCurve;

    void InitializeMarkers();
    void Clear();
public:
    //true for plane, false for cyllinder
    bool isPlane;
    //cylinder
    int R;
    int H;
    //plane
    float Width;
    float Height;
    //common
    int X;
    int Y;
    int U;
    int V;
    QVector4D offset;

    BSplinePlane();
    BSplinePlane(QMatrix4x4 matrix);
    BSplinePlane(QMatrix4x4 matrix, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane);
    QList<BicubicSegment> BezierSegments;
    QVector<QList<Marker*>> BezierSegMarkers;
    //QList<Marker*[]> BezierSegMarkers;

    void InitializeSpline(QMatrix4x4 matrix);
    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);


    //void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker> getMarkers();
};

#endif // BSPLINEPLANE_H
