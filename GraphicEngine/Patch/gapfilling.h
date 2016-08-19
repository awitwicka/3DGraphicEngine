#ifndef GAPFILLING_H
#define GAPFILLING_H

#include "cadsplinepatch.h"
#include "gregorypatch.h"


class GapFilling : public CADObject, public CADSplinePatch
{
    static int id;

    QList<CADSplinePatch*> patches;
    QVector<QVector4D> pointsVectors;
    QVector<QPoint> indicesVectors;

    Marker *FindFirstCommonMarker(CADSplinePatch* patch1, CADSplinePatch* patch2);
    //returns 7 points after performing algorithm on each of this point
    QList<QVector4D> DeCasteljau(QVector4D pkt[ORDER]);
    //returns points on the edges of a tree
    QList<QVector4D> DeCasteljauTree(QVector4D pkt[ORDER], float t);
    //QList<GregoryPatch> gregPatches;
public:
    GapFilling();
    GapFilling(QMatrix4x4 matrix, QList<CADSplinePatch*> patches);
    GapFilling(QMatrix4x4 matrix, CADSplinePatch *patch1, CADSplinePatch *patch2, CADSplinePatch *patch3);

    void InitializeSpline(QMatrix4x4 matrix);
    void Clear();
    void DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
private:
    //get border control points that can be calculated based on the 'patch'
    QVector4D ComputeBorderControlPoints(Marker* a, Marker* c, CADSplinePatch* patch, QList<QVector4D> &cp0, QList<QVector4D> &cp1);
    QList<QVector4D> ComputeMiddleControlPoints(QVector4D b0, QVector4D a3, QVector4D b3, QVector4D Bezier[ORDER]);
    void GetFirst2Lines(Marker* a, Marker* c, CADSplinePatch* patch, QVector4D *line1, QVector4D *line2);
};

#endif // GAPFILLING_H
