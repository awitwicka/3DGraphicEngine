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
    QList<GregoryPatch> gregPatches;

    //     midP
    //      |
    //  ____|_____
    // | a2 d2 b2 |
    // |    |     |
    // |_a1_c1_b1_|
    //      |
    //______c0______
    //
    //  ____
    // |____| CPmiddle |
    // ______ CPline1  |
    // ______ CPline0  |
    //              Bezier
    //
    QList<QList<Marker> > CPline0; //try qlist of arays insted?
    QList<QList<Marker> > CPline1;
    QList<QList<Marker> > Bezier;
    QList<QList<Marker> > CPmiddle;

    void InitGregory(QMatrix4x4 matrix);
    //void InitializeMarkers(QList<Marker> *MainMarkers);
    Marker *FindFirstCommonMarker(CADSplinePatch* patch1, CADSplinePatch* patch2);
    //returns 7 points after performing algorithm on each of this point
    QList<QVector4D> DeCasteljau(QVector4D pkt[ORDER]);
    //returns points on the edges of a tree
    QList<QVector4D> DeCasteljauTree(QVector4D pkt[ORDER], float t);
public:
    //consider removing mainmarkers ref
    GapFilling();
    GapFilling(QMatrix4x4 matrix, QList<Marker> *MainMarkers, float U, float V, QList<CADSplinePatch*> patches);
    GapFilling(QMatrix4x4 matrix, QList<Marker> *MainMarkers, float U, float V, CADSplinePatch *patch1, CADSplinePatch *patch2, CADSplinePatch *patch3);

    void InitializeSpline(QMatrix4x4 matrix);
    void Clear();
    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    void setU(int value);
    void setV(int value);
private:
    //get border control points that can be calculated based on the 'patch'
    QVector4D ComputeBorderControlPoints(Marker* a, Marker* c, CADSplinePatch* patch, QList<QVector4D> &cp0, QList<QVector4D> &cp1);
    QList<QVector4D> ComputeMiddleControlPoints(QVector4D b0, QVector4D a3, QVector4D b3, QVector4D Bezier[ORDER]);
    void GetFirst2Lines(Marker* a, Marker* c, CADSplinePatch* patch, QVector4D *line1, QVector4D *line2);
};

#endif // GAPFILLING_H
