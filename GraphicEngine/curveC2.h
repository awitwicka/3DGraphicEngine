#ifndef CURVEC2_H
#define CURVEC2_H

#include "cadobject.h"
#include "marker.h"
#include "segment.h"

class CurveC2 : public CADObject
{
    static int id;
    int degree;
    QList<Segment> BezierSegments;
    QVector<QVector4D> points;
    QVector<QPoint> indices;
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    void Clear();
    QVector4D getBezierPoint(Segment seg, float t);
    float getBSplineWeight(float t, int i, int k, QList<int> knots);

    //void getCurveGeometry();
public:

    QString name;
    QString idname;
    bool isBezier;
    QList<Marker*> boorMarkers;
    QList<Marker> bezierMarkers; //punkty 'virtualne'

    CurveC2();
    CurveC2(QList<Marker*> const & m, QMatrix4x4 matrix);
    void InitializeBezier(QMatrix4x4 matrix);
    void InitializeBSpline(QMatrix4x4 matrix);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void ChangeToOpposite();
    void ChangeToBezier();
    void AdjustOtherPoints();

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
};

#endif // CURVEC2_H
