#include "bezierplane.h"

#define DEGREE 3

BezierPlane::BezierPlane()
{

}

void BezierPlane::InitializeSpline(QMatrix4x4 matrix)
{
    markers.clear();
    //markers
}

QVector<QPoint> BezierPlane::getIndices() const
{
    return indices;
}

void BezierPlane::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> BezierPlane::getPoints() const
{
    return points;
}

void BezierPlane::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

QList<Marker *> BezierPlane::getMarkers() const
{
    return markers;
}
