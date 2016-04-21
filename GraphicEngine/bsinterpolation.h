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

    //void Nval(float N[], int row, int col);
    QList<Marker> ControlPoints;
    QList<float> parameters;
    QList<float> knots;  
    void CalculateParameters();
    void CalculateControlPoints();
    float getBSplineWeight(float t, int i, int k, QList<float> knots, int n);

    void LU(float **a, unsigned long n, int m1, int m2, float **al, unsigned long indx[], float *d);
    void solveEq(float **a, unsigned long n, int m1, int m2, float **al, unsigned long indx[], float b[]);
public:
    BSInterpolation();
    BSInterpolation(QList<Marker*> const & m, QMatrix4x4 matrix);
    //TODO: virtualize
    void InitializeInterpolation(QMatrix4x4 matrix);
    void InitializeBSpline(QMatrix4x4 matrix);
    QVector4D getBezierPoint(Segment seg, float t);

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
    QList<Marker *> getMarkers() const;
};

#endif // BSPLINEINTERPOLATION_H
