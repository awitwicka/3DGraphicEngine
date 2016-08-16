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
    QList<QVector4D> DeCasteljau(QVector4D pkt[ORDER]);
    //QList<GregoryPatch> gregPatches;
    //QList<>
public:
    GapFilling();
    GapFilling(QMatrix4x4 matrix, QList<CADSplinePatch*> patches);
    GapFilling(QMatrix4x4 matrix, CADSplinePatch *patch1, CADSplinePatch *patch2, CADSplinePatch *patch3);

    void InitializeSpline(QMatrix4x4 matrix);
    void Clear();
    void DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
private:
    void GetFirst2Lines(Marker* a, Marker* c, CADSplinePatch* patch, QVector4D *line1, QVector4D *line2);
};

#endif // GAPFILLING_H
