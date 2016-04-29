#ifndef BICUBICSEGMENT_H
#define BICUBICSEGMENT_H

#include "marker.h"
#define DEGREE 3
#define ORDER DEGREE+1

class BicubicSegment : CADObject
{
    //-1 for lack of data
    QVector4D getBezierPoint(QVector4D points[ORDER], float t);
    QVector4D getBezierPointCol(int index, float t);
    QVector4D getBezierPointRow(int index, float t);
    void Clear();
public:
    int U = 4;
    int V = 4;
    float segLength;
    BicubicSegment();
    BicubicSegment(QList<Marker*> m, int u, int v, float length);
    void InitializeSpline(QMatrix4x4 matrix);
    int pointsNo;
    Marker* markers[ORDER][ORDER];
    //QList<Marker*> markers;

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker *> getMarkers() const;

};

#endif // BICUBICSEGMENT_H
