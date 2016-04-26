#ifndef CURVEC2_H
#define CURVEC2_H

#include "cadobject.h"
#include "marker.h"
#include "segment.h"
#include "cadmarkerobject.h"

class CurveC2 : public CADObject, public CADMarkerObject
{
    static int id;
    int degree;
    QList<Segment> BezierSegments;
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;
    //QVector<QVector4D> pointsCurve2;
    //QVector<QPoint> indicesCurve2;

    void Clear();
    QVector4D getBezierPoint(Segment seg, float t);
    float getBSplineWeight(float t, int i, int k, QList<int> knots);

    //void getCurveGeometry();
    struct VirtualMarkInfo {
        QVector4D position;
        Marker* parent;
        Marker* partner;
    };
public:
    bool isBezier;
    QList<Marker> bezierMarkers; //punkty 'virtualne'

    CurveC2();
    CurveC2(QList<Marker*> const & m, QMatrix4x4 matrix);
    void InitializeBezierC2(QMatrix4x4 matrix);
    void InitializeSpline(QMatrix4x4 matrix);
    //same as change to bezier but filling list with new points instead of substituting
    void InitializeBezierMarkers();
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void ChangeToOpposite();
    void ChangeToBezier();
    void AdjustOtherPoints(Marker* m, QVector4D oldPosition);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker *> getMarkers() const;
};

#endif // CURVEC2_H
