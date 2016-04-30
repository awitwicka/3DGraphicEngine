#ifndef BICUBICSEGMENT_H
#define BICUBICSEGMENT_H

#include "cadsplinepatch.h"
#include "marker.h"
#define DEGREE 3
#define ORDER 4

class BicubicSegment : public CADObject, public CADSplinePatch
{
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    QVector4D getBezierPoint(QVector4D points[ORDER], float t);
    QVector4D getBezierPointCol(int index, float t);
    QVector4D getBezierPointRow(int index, float t);
    void Clear();
public:
    int U;
    int V;
    //float segLengthX;
    //float segLengthY;
    BicubicSegment();
    BicubicSegment(QList<Marker*>* m, int u, int v, QMatrix4x4 matrix);
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
