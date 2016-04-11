#ifndef BSPLINE_H
#define BSPLINE_H

#include "cadobject.h"
#include "marker.h"

class BSpline : public CADObject
{
    static int id;
    float getBSplineWeight(float t, int i, int k, QList<int> knots);
    void getCurveGeometry();

    QVector<QVector4D> points;
    QVector<QPoint> indices;
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    void ChangeToBezier();
    void Clear();
public:
    void InitializeBSpline(QMatrix4x4 matrix);
    QString name;
    QString idname;
    BSpline();
    QList<Marker*> markers;
    BSpline(QList<Marker*> const & m, QMatrix4x4 matrix);
    void DrawCurve(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
};

#endif // BSPLINE_H
