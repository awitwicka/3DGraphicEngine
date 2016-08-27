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

    //Gradient descend method - finding first intersection point

    //function f(u1v1u2v2) = sqrt((g.x-h.x)^2 + (g.y-h.y)^2 + (g.z-h.z)^2)
    //subfunctions g(u1v1) - patch1->ComputePos(u, v);
    //subfunctions h(u2v2) - patch2->ComputePos(u, v);
    //gradient d = 2*(g.x-h.x)*(g.x'-h.x') + 2*(g.y-h.y)*(g.y'-h.y') + 2*(g.z-h.z)*(g.z'-h.z') /
    //             / (2*f(u1v1u2v2))

    int count = indicesCurve.length()/2+1;
    //in terms of uv
    e=0.01f;
    float a = 0.01f; //step
    QVector4D x = QVector4D(point1.u, point1.v, point2.u, point2.v); //start point
    QVector4D new_x = x;
    float stopCond;
    float new_f;
    do {
        pointsCurve.append(patch1->ComputePos(x.x(), x.y()));
        indicesCurve.append(QPoint(count, count+1));
        count +=1;

        //get f
        QVector4D g = patch1->ComputePos(x.x(), x.y()); //u1 v1
        QVector4D h = patch2->ComputePos(x.z(), x.w()); //u2 v2
        float f = sqrt(pow((g.x()-h.x()),2) + pow((g.y()-h.y()),2) + pow((g.z()-h.z()),2)); // u1 v1 u2 v2 - distance between 2 points
        //get grad
        QVector4D gdu = patch1->ComputeDu(x.x(), x.y()); //u1' v1
        QVector4D gdv = patch1->ComputeDv(x.x(), x.y()); //u1 v1'
        QVector4D hdu = patch1->ComputeDu(x.z(), x.w()); //u2' v2
        QVector4D hdv = patch1->ComputeDv(x.z(), x.w()); //u2 v2'
        QVector4D d;
        d.setX( (2*(g.x()-h.x())*(gdu.x()) + 2*(g.y()-h.y())*(gdu.y()) + 2*(g.z()-h.z())*(gdu.z())) / (2*f) ); //du1
        d.setY( (2*(g.x()-h.x())*(gdv.x()) + 2*(g.y()-h.y())*(gdv.y()) + 2*(g.z()-h.z())*(gdv.z())) / (2*f) ); //dv1
        d.setZ( (2*(g.x()-h.x())*(-hdu.x()) + 2*(g.y()-h.y())*(-hdu.y()) + 2*(g.z()-h.z())*(-hdu.z())) / (2*f) ); //du2
        d.setW( (2*(g.x()-h.x())*(-hdv.x()) + 2*(g.y()-h.y())*(-hdv.y()) + 2*(g.z()-h.z())*(-hdv.z())) / (2*f) ); //dv2

        new_x = x - (a*d);
        QVector4D new_g = patch1->ComputePos(new_x.x(), new_x.y()); //u1 v1
        QVector4D new_h = patch2->ComputePos(new_x.z(), new_x.w()); //u2 v2
        new_f = sqrt(pow((new_g.x()-new_h.x()),2) + pow((new_g.y()-new_h.y()),2) + pow((new_g.z()-new_h.z()),2));

        stopCond = (new_x - x).length();
        if (new_f >= f) a /= 2;
        else x = new_x;
    } while(stopCond > e); //is length already abs? - pkt stabilny
    pointsCurve.append(patch1->ComputePos(x.x(), x.y()));

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
