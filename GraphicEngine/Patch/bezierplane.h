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

    //return indexes of corresponding bezier segment
    QList<int> ConvertUVtoLocal(float &u, float &v);

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

    BezierPlane();
    BezierPlane(QMatrix4x4 matrix, QList<Marker> *markers);
    //Direction 0 - horizontal, 1 - vertical, N - no of markers at x axis, M - no of markers at Z axis
    BezierPlane(QMatrix4x4 matrix, QList<Marker*> markers, float X, float Y, bool isHorizontal, bool isPlane/*, QString name*/);
    BezierPlane(QMatrix4x4 matrix, QList<Marker> *markers, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane);
    //QList<BicubicSegment> BezierSegments;
    QVector<QList<Marker*>> BezierSegMarkers;
    //QList<Marker*[]> BezierSegMarkers;

    void InitializeSpline(QMatrix4x4 matrix);
    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void Clear();
    void ReplaceMarker(Marker *toReplace, Marker *replaceWith);
    QVector4D ComputePos(float u, float v);
    QVector4D ComputeDu(float u, float v);
    QVector4D ComputeDv(float u, float v);
    QVector4D ComputeDuv(float u, float v);
    QVector4D ComputeDvu(float u, float v);
    QVector4D ComputeDuu(float u, float v);
    QVector4D ComputeDvv(float u, float v);

    //void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker*> getMarkers();
};

#endif // BEZIERPLANE_H
