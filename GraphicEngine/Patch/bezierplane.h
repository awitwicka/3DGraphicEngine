#ifndef BEZIERPLANE_H
#define BEZIERPLANE_H

#include "cadobject.h"
#include "bicubicsegment.h"
#include "cadsplinepatch.h"

class BezierPlane : public CADObject, public CADSplinePatch
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
    //plane
    float Width;
    float Height;
    //common
    int X;
    int Y;
    //int U;
    //int V;
    QVector4D offset;

    BezierPlane();
    BezierPlane(QMatrix4x4 matrix, QList<Marker> *markers);
    BezierPlane(QMatrix4x4 matrix, QList<Marker> *markers, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane);
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

#endif // BEZIERPLANE_H
