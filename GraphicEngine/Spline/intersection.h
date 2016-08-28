#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "cadmarkerobject.h"
#include <Patch/cadsplinepatch.h>


class Intersection : public CADObject, public CADMarkerObject
{
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    struct UVPointData {
        QVector4D position;
        float u;
        float v;

        UVPointData(QVector4D pos, float u, float v) : position(pos), u(u), v(v) {}
    };

    void Clear();
    //find closest point on the surface on both patches
    UVPointData FindClosesPointOnSurface(QVector4D PointPos, CADSplinePatch *patch, float accuracy);
    QVector4D NewtonNextPoint(QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2);
    QVector4D GradientDistanceMinimalization(float e, float a, QVector4D x, CADSplinePatch *patch1, CADSplinePatch *patch2);
public:
    Intersection();
    Intersection(QMatrix4x4 matrix, Marker* start, CADSplinePatch *patch1, CADSplinePatch *patch2);

    void InitializeSpline(QMatrix4x4 matrix);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
};

#endif // INTERSECTION_H
