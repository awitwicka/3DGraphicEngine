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

    void InitializeMarkers(QList<Marker> *MainMarkers);
public:
    //true for plane, false for cyllinder
    //bool isPlane;
    //cylinder
    int R;
    int H;
    //common
    int X;
    int Y;
    //int U;
    //int V;
    QVector4D offset;

    BSplinePlane();
    BSplinePlane(QMatrix4x4 matrix, QList<Marker> *MainMarkers);
    BSplinePlane(QMatrix4x4 matrix, QList<Marker*> markers, float X, float Y, bool isHorizontal, bool isPlane/*, QString name*/);
    BSplinePlane(QMatrix4x4 matrix, QList<Marker> *MainMarkers, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane);
    //QList<BicubicSegment> BezierSegments;
    QVector<QList<Marker*>> BezierSegMarkers;
    //QList<Marker*[]> BezierSegMarkers;

    void InitializeSpline(QMatrix4x4 matrix);
    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void Clear();
    void ReplaceMarker(Marker *toReplace, Marker *replaceWith);

    //void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker*> getMarkers();
};

#endif // BSPLINEPLANE_H
