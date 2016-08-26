#include "intersection.h"

Intersection::Intersection()
{

}

Intersection::Intersection(QMatrix4x4 matrix, Marker* start, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    //step accuracy
    float e = 0.01f;
    UVPointData point1 = FindClosesPointOnSurface(start->point, patch1, e);
    UVPointData point2 = FindClosesPointOnSurface(start->point, patch2, e);

    //tmp
    pointsCurve.append(start->point);
    pointsCurve.append(point1.position);
    pointsCurve.append(start->point);
    pointsCurve.append(point2.position);
    indicesCurve.append(QPoint(0, 1));
    indicesCurve.append(QPoint(2, 3));

    //Conjugate gradient method - finding first intersection point
    /*float r0 = fabs((point1.position - point2.position).length()); //dist we want to minimize
    float p0 = r0;
    int k =0;
    while(r0 < e) {
        int alpha =
    }*/

    //for (int i = 0; i<patch1->)
    //kliknij na powierzchni 1
    //losuj pkt na powierzchni 2
    //minimalizuj odleglosci
}

Intersection::UVPointData Intersection::FindClosesPointOnSurface(QVector4D PointPos, CADSplinePatch *patch, float accuracy)
{
    QVector4D pos;
    float dist;
    float Uval;
    float Vval;

    //int count = indicesCurve.length()/2; //+1 jak chcemy miec przerwe
    //TODO: optimize
    float tmpDist;
    QVector4D tmpPos;
    pos = patch->ComputePos(0, 0);
    dist = fabs((PointPos - pos).length());
    for (float u = 0; u <= 1; u+=accuracy) {
        for (float v = 0; v <= 1; v+=accuracy) {
            tmpPos = patch->ComputePos(u, v);
            tmpDist = fabs((PointPos - tmpPos).length());
            if (dist>tmpDist) {
                dist = tmpDist;
                Uval = u;
                Vval = v;
                pos = tmpPos;
           }
           /*pointsCurve.append(tmpPos);
           indicesCurve.append(QPoint(count, count+1));
           count +=1;*/
        }
    }
    //pointsCurve.append(tmpPos);
    return UVPointData(pos, Uval, Vval);
}

void Intersection::InitializeSpline(QMatrix4x4 matrix)
{

}

void Intersection::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::red;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

void Intersection::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
}
