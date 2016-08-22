#ifndef GREGORYPATCH_H
#define GREGORYPATCH_H

#include "cadsplinepatch.h"



class GregoryPatch : public CADObject, public CADSplinePatch
{
    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    /*Marker* TR[2]; //top right points
    Marker* TL[2]; //top left points
    Marker* BR[2]; //bottom right points
    Marker* BL[2]; //bottom left points*/
public:
    GregoryPatch();
    GregoryPatch(QList<Marker>* m, int u, int v, QMatrix4x4 matrix);
    GregoryPatch(QList<Marker*> gapMarkers, int u, int v, QMatrix4x4 matrix);

    void Clear();
    void InitializeMarkers();
    void InitializeSpline(QMatrix4x4 matrix);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    QVector4D getBezierPoint(QVector4D points[ORDER], float t);
    QVector4D getBezierPointCol(int index, float u, float v);
    QVector4D getBezierPointRow(int index, float u, float v);
};

#endif // GREGORYPATCH_H
