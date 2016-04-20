#ifndef BSPLINEINTERPOLATION_H
#define BSPLINEINTERPOLATION_H

#include "cadmarkerobject.h"
#include "segment.h"


class BSInterpolation : public CADObject, public CADMarkerObject
{
    static int id;
    int degree = 3;

    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;
    void Clear();

    QList<Marker*> DataPoints;
    QList<Marker> ControlPoints;
    QList<float> parameters;
    QList<float> knots;  
    void CalculateParameters();
    void CalculateControlPoints();
    float getBSplineWeight(float t, int i, int k, QList<float> knots, int n);
public:
    BSInterpolation();
    BSInterpolation(QList<Marker*> const & m, QMatrix4x4 matrix);
    //TODO: virtualize
    QList<Segment> BezierSegments;
     void InitializeInterpolation(QMatrix4x4 matrix);
    void InitializeBSpline(QMatrix4x4 matrix);
    QVector4D getBezierPoint(Segment seg, float t);
};

#endif // BSPLINEINTERPOLATION_H
