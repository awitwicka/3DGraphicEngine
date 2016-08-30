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
    /*pointsCurve.append(start->point);
    pointsCurve.append(point1.position);
    pointsCurve.append(start->point);
    pointsCurve.append(point2.position);
    indicesCurve.append(QPoint(0, 1));
    indicesCurve.append(QPoint(2, 3));
    */

    //Gradient descend method - finding first intersection point
    float a = 0.01f; //step
    QVector4D x = QVector4D(point1.u, point1.v, point2.u, point2.v); //start point u1v1u2v2
    QVector4D x_optimal = GradientDistanceMinimalization(e, a, x, patch1, patch2);
    //debug
    //QVector4D g_point = patch1->ComputePos(x_optimal.x(), x_optimal.y());
    //QVector4D h_point = patch2->ComputePos(x_optimal.z(), x_optimal.w());

    int count = 0;
    QVector4D point = x_optimal;
    for (int i=0; i<200; i++) {
        //QVector4D nextStep = GradientStep(e, a, point, patch1, patch2);
        //point = GradientDistanceMinimalization(e, a, nextStep, patch1, patch2);

        point = NewtonNextPoint(point, patch1, patch2);
        pointsCurve.append(patch1->ComputePos(point.x(), point.y()));
        indicesCurve.append(QPoint(count, count+1));
        count +=1;
    }
    pointsCurve.append(patch1->ComputePos(point.x(), point.y()));
}

QVector4D Intersection::GradientDistanceMinimalization(float e, float a, QVector4D x, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    //function f(u1v1u2v2) = sqrt((g.x-h.x)^2 + (g.y-h.y)^2 + (g.z-h.z)^2)
    //subfunctions g(u1v1) - patch1->ComputePos(u, v);
    //subfunctions h(u2v2) - patch2->ComputePos(u, v);
    //gradient d = 2*(g.x-h.x)*(g.x'-h.x') + 2*(g.y-h.y)*(g.y'-h.y') + 2*(g.z-h.z)*(g.z'-h.z') /
    //             / (2*f(u1v1u2v2))

    //int count = indicesCurve.length()/2;
    QVector4D new_x = x;
    float stopCond;
    float new_f;
    do {
        //pointsCurve.append(patch1->ComputePos(x.x(), x.y()));
        //indicesCurve.append(QPoint(count, count+1));
        //count +=1;

        //get f
        QVector4D g = patch1->ComputePos(x.x(), x.y()); //u1 v1
        QVector4D h = patch2->ComputePos(x.z(), x.w()); //u2 v2
        float f = sqrt(pow((g.x()-h.x()),2) + pow((g.y()-h.y()),2) + pow((g.z()-h.z()),2)); // u1 v1 u2 v2 - distance between 2 points
        //get grad
        QVector4D gdu = patch1->ComputeDu(x.x(), x.y()); //u1' v1
        QVector4D gdv = patch1->ComputeDv(x.x(), x.y()); //u1 v1'
        QVector4D hdu = patch2->ComputeDu(x.z(), x.w()); //u2' v2
        QVector4D hdv = patch2->ComputeDv(x.z(), x.w()); //u2 v2'
        QVector4D d;
        d.setX( (2*(g.x()-h.x())*(gdu.x()) + 2*(g.y()-h.y())*(gdu.y()) + 2*(g.z()-h.z())*(gdu.z())) / (2*f) ); //du1
        d.setY( (2*(g.x()-h.x())*(gdv.x()) + 2*(g.y()-h.y())*(gdv.y()) + 2*(g.z()-h.z())*(gdv.z())) / (2*f) ); //dv1
        d.setZ( (2*(g.x()-h.x())*(-hdu.x()) + 2*(g.y()-h.y())*(-hdu.y()) + 2*(g.z()-h.z())*(-hdu.z())) / (2*f) ); //du2
        d.setW( (2*(g.x()-h.x())*(-hdv.x()) + 2*(g.y()-h.y())*(-hdv.y()) + 2*(g.z()-h.z())*(-hdv.z())) / (2*f) ); //dv2

        //TODO: minimalizacja kierunkowa
        //find a such that f(x - (a*d)) is mininum

        //get new x
        new_x = x - (a*d);
        //get new f
        QVector4D new_g = patch1->ComputePos(new_x.x(), new_x.y()); //u1 v1
        QVector4D new_h = patch2->ComputePos(new_x.z(), new_x.w()); //u2 v2
        new_f = sqrt(pow((new_g.x()-new_h.x()),2) + pow((new_g.y()-new_h.y()),2) + pow((new_g.z()-new_h.z()),2));

        stopCond = (new_x - x).length();
        if (new_f >= f) a /= 2;
        else x = new_x;
    } while(stopCond > e); //is length already abs? - pkt stabilny
    //pointsCurve.append(patch1->ComputePos(x.x(), x.y()));
    return new_x;
}

QVector4D Intersection::GradientStep(float e, float a, QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    float new_f;
    float stopCond;
    float step = 1.0f;
    int turn = 1;

    QVector3D g0 = QVector3D(patch1->ComputePos(startPoint.x(), startPoint.y())); //u1 v1
    QVector3D h0 = QVector3D(patch2->ComputePos(startPoint.z(), startPoint.w())); //u2 v2
    QVector3D gdu0 = QVector3D(patch1->ComputeDu(startPoint.x(), startPoint.y())); //u1' v1
    QVector3D gdv0 = QVector3D(patch1->ComputeDv(startPoint.x(), startPoint.y())); //u1 v1'
    QVector3D hdu0 = QVector3D(patch2->ComputeDu(startPoint.z(), startPoint.w())); //u2' v2
    QVector3D hdv0 = QVector3D(patch2->ComputeDv(startPoint.z(), startPoint.w())); //u2 v2'

    QMatrix4x4 H; //Hessian matrix
    QVector4D d; //gradinet of the function
    QVector3D Ng;
    QVector3D Nh;
    QVector3D dir;

    //normal to the surface
    Ng = QVector3D::crossProduct(gdu0, gdv0);
    Nh = QVector3D::crossProduct(hdu0, hdv0);
    //searching direction
    dir = (QVector3D::crossProduct(Ng, Nh)).normalized();
    dir *= turn;

    QVector4D x = startPoint;
    QVector4D new_x = x;
    do {
        QVector3D g = QVector3D(patch1->ComputePos(x.x(), x.y())); //u1 v1
        QVector3D gdu = QVector3D(patch1->ComputeDu(x.x(), x.y())); //u1' v1
        QVector3D gdv = QVector3D(patch1->ComputeDv(x.x(), x.y())); //u1 v1'

        QVector3D h = QVector3D(patch2->ComputePos(x.z(), x.w())); //u2 v2
        QVector3D hdu = QVector3D(patch2->ComputeDu(x.z(), x.w())); //u2' v2
        QVector3D hdv = QVector3D(patch2->ComputeDv(x.z(), x.w())); //u2 v2'

        float f = pow((QVector3D::dotProduct(g-g0, dir) - step),2) + pow((QVector3D::dotProduct(h-h0, dir) - step),2);
        //grad
        d.setX( 2*(QVector3D::dotProduct(g-g0, dir) - step) * QVector3D::dotProduct(dir, gdu) ); //du1
        d.setY( 2*(QVector3D::dotProduct(g-g0, dir) - step) * QVector3D::dotProduct(dir, gdv) ); //dv1
        d.setZ( 2*(QVector3D::dotProduct(h-h0, dir) - step) * QVector3D::dotProduct(dir, hdu) ); //du2
        d.setW( 2*(QVector3D::dotProduct(h-h0, dir) - step) * QVector3D::dotProduct(dir, hdv) ); //dv2

        //TODO: minimalizacja kierunkowa
        //find a such that f(x - (a*d)) is mininum

        //get new x
        new_x = x - (a*d);
        //get new f
        QVector3D new_g = QVector3D(patch1->ComputePos(new_x.x(), new_x.y())); //u1 v1
        QVector3D new_h = QVector3D(patch2->ComputePos(new_x.z(), new_x.w())); //u2 v2
        new_f = pow((QVector3D::dotProduct(new_g-g0, dir) - step),2) + pow((QVector3D::dotProduct(new_h-h0, dir) - step),2);

        stopCond = (new_x - x).length();
        if (new_f >= f) a /= 2;
        else x = new_x;
    } while(stopCond > e); //is length already abs? - pkt stabilny
    return new_x;
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
    Uval = 0;
    Vval = 0;
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

QVector4D Intersection::NewtonNextPoint(QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2) //u1v1u2v2 point
{
    float stopCond;
    float e = 0.01f;
    float step = 1.0f; //in xyz
    //float step = 0.01f; //TO DO: pass to intersection in constructor
    int max_iter = 10;
    int i = 0;
    int turn = 1;
    // if backwards then
    // turn = -1;

    //g0 start
    QVector3D g0 = QVector3D(patch1->ComputePos(startPoint.x(), startPoint.y())); //u1 v1
    QVector3D gdu0 = QVector3D(patch1->ComputeDu(startPoint.x(), startPoint.y())); //u1' v1
    QVector3D gdv0 = QVector3D(patch1->ComputeDv(startPoint.x(), startPoint.y())); //u1 v1'
    QVector3D gdvu0 = QVector3D(patch1->ComputeDvu(startPoint.x(), startPoint.y())); //u1' v1'
    QVector3D gduv0 = QVector3D(patch1->ComputeDuv(startPoint.x(), startPoint.y())); //u1' v1'
    QVector3D gduu0 = QVector3D(patch1->ComputeDuu(startPoint.x(), startPoint.y())); //u1'' v1
    QVector3D gdvv0 = QVector3D(patch1->ComputeDvv(startPoint.x(), startPoint.y())); //u1 v1''
    //h0 start
    QVector3D h0 = QVector3D(patch2->ComputePos(startPoint.z(), startPoint.w())); //u2 v2
    QVector3D hdu0 = QVector3D(patch2->ComputeDu(startPoint.z(), startPoint.w())); //u2' v2
    QVector3D hdv0 = QVector3D(patch2->ComputeDv(startPoint.z(), startPoint.w())); //u2 v2'
    QVector3D hdvu0 = QVector3D(patch2->ComputeDvu(startPoint.z(), startPoint.w())); //u2' v2'
    QVector3D hduv0 = QVector3D(patch2->ComputeDuv(startPoint.z(), startPoint.w())); //u2' v2'
    QVector3D hduu0 = QVector3D(patch2->ComputeDuu(startPoint.z(), startPoint.w())); //u2'' v2
    QVector3D hdvv0 = QVector3D(patch2->ComputeDvv(startPoint.z(), startPoint.w())); //u2 v2''

    QVector4D x_new;
    QMatrix4x4 H; //Hessian matrix
    QVector4D d; //gradinet of the function
    QVector3D Ng;
    QVector3D Nh;
    QVector3D dir;

    //normal to the surface
    Ng = QVector3D::crossProduct(gdu0, gdv0);
    Nh = QVector3D::crossProduct(hdu0, hdv0);
    //searching direction
    dir = (QVector3D::crossProduct(Ng, Nh)).normalized();
    dir *= turn;
    //derivative of dir
    //ng.y*nh.z - ng.z*nh.y -> -> cross(nh, cross(gu, gv)) -> cross(nh, cross(guu, gv) + cross(gu, gvu))
    //ng.z*nh.x - ng.x*nh.z ->
    //ng.x*nh.y - ng.y*nh.x ->
    QVector3D dirU1 = QVector3D::crossProduct(Nh, (QVector3D::crossProduct(gduu0, gdv0) + QVector3D::crossProduct(gdu0,gdvu0)));
    QVector3D dirV1 = QVector3D::crossProduct(Nh, (QVector3D::crossProduct(gduv0, gdv0) + QVector3D::crossProduct(gdu0,gdvv0)));
    QVector3D dirU2 = QVector3D::crossProduct(Ng, (QVector3D::crossProduct(hduu0, hdv0) + QVector3D::crossProduct(hdu0,hdvu0)));
    QVector3D dirV2 = QVector3D::crossProduct(Ng, (QVector3D::crossProduct(hduv0, hdv0) + QVector3D::crossProduct(hdu0,hdvv0)));
    dirU1 *= turn;
    dirV1 *= turn;
    dirU2 *= turn;
    dirV2 *= turn;

    dirU1 = QVector3D(0,0,0);
    dirV1 = QVector3D(0,0,0);
    dirU2 = QVector3D(0,0,0);
    dirV2 = QVector3D(0,0,0);

    QVector3D dirU1U1 = QVector3D(0,0,0);
    QVector3D dirU1V1 = QVector3D(0,0,0);
    QVector3D dirU1U2 = QVector3D(0,0,0);
    QVector3D dirU1V2 = QVector3D(0,0,0);

    QVector3D dirV1U1 = QVector3D(0,0,0);
    QVector3D dirV1V1 = QVector3D(0,0,0);
    QVector3D dirV1U2 = QVector3D(0,0,0);
    QVector3D dirV1V2 = QVector3D(0,0,0);

    QVector3D dirU2U1 = QVector3D(0,0,0);
    QVector3D dirU2V1 = QVector3D(0,0,0);
    QVector3D dirU2U2 = QVector3D(0,0,0);
    QVector3D dirU2V2 = QVector3D(0,0,0);

    QVector3D dirV2U1 = QVector3D(0,0,0);
    QVector3D dirV2V1 = QVector3D(0,0,0);
    QVector3D dirV2U2 = QVector3D(0,0,0);
    QVector3D dirV2V2 = QVector3D(0,0,0);

    QVector4D x = startPoint;
    do{
        QVector3D g = QVector3D(patch1->ComputePos(x.x(), x.y())); //u1 v1
        QVector3D gdu = QVector3D(patch1->ComputeDu(x.x(), x.y())); //u1' v1
        QVector3D gdv = QVector3D(patch1->ComputeDv(x.x(), x.y())); //u1 v1'
        QVector3D gdvu = QVector3D(patch1->ComputeDvu(x.x(), x.y())); //u1' v1'
        QVector3D gduv = QVector3D(patch1->ComputeDuv(x.x(), x.y())); //u1' v1'
        QVector3D gduu = QVector3D(patch1->ComputeDuu(x.x(), x.y())); //u1'' v1
        QVector3D gdvv = QVector3D(patch1->ComputeDvv(x.x(), x.y())); //u1 v1''

        QVector3D h = QVector3D(patch2->ComputePos(x.z(), x.w())); //u2 v2
        QVector3D hdu = QVector3D(patch2->ComputeDu(x.z(), x.w())); //u2' v2
        QVector3D hdv = QVector3D(patch2->ComputeDv(x.z(), x.w())); //u2 v2'
        QVector3D hdvu = QVector3D(patch2->ComputeDvu(x.z(), x.w())); //u2' v2'
        QVector3D hduv = QVector3D(patch2->ComputeDuv(x.z(), x.w())); //u2' v2'
        QVector3D hduu = QVector3D(patch2->ComputeDuu(x.z(), x.w())); //u2'' v2
        QVector3D hdvv = QVector3D(patch2->ComputeDvv(x.z(), x.w())); //u2 v2''
        //g_x h_x
        /*QVector3D g = QVector3D(patch1->ComputePos(x.x(), x.y()));
        QVector3D h = QVector3D(patch2->ComputePos(x.z(), x.w()));
        QVector3D gdu = QVector3D(patch1->ComputeDu(x.x(), x.y()));
        QVector3D gdv = QVector3D(patch1->ComputeDv(x.x(), x.y()));
        QVector3D hdu = QVector3D(patch2->ComputeDu(x.z(), x.w()));
        QVector3D hdv = QVector3D(patch2->ComputeDv(x.z(), x.w()));*/

        /***********OK*************/
        //plane normal to 'dir' at distance 'step' from g0, that consists of point g
        QVector3D distG = g-g0;
        //QVector3D plane = QVector3D::dotProduct(distG, dir) - step; // = 0
        //find g_new such that g_new = h_new and n_new is also on a plane (dotProduct(g_new-g0, dir) - step = 0)
        //newton method: u1v1u2v2 - > new_u1v1u2v2
        //1)new_u1v1 = new_u2v2 -> funkcja celu minimalizacja odleglosci
        //2)dotProduct(u1v1-g0, dir) - step = 0
        // (g0 = h0)
        //function f(u1v1u2v2) = dotProduct(g_new-g0, dir) - step = 0 +
        //function f(u1v1u2v2) = dotProduct(h_new-g0, dir) - step = 0
        //spadek na ten sam pkt
        //lub dotProduct(g_new-g0, dir) - step + spadek

        float DistDir = QVector3D::dotProduct(distG, dir) - step;
        float f = pow((g.x()-h.x()),2) + pow((g.y()-h.y()),2) + pow((g.z()-h.z()),2) + pow(DistDir,2); // u1 v1 u2 v2 - distance between 2 points
        //grad
        float dU1_DistDir = /*QVector3D::dotProduct(dirU1, distG) + */QVector3D::dotProduct(dir, gdu/*-gdu0*/);
        float dV1_DistDir = /*QVector3D::dotProduct(dirV1, distG) +*/ QVector3D::dotProduct(dir, gdv/*-gdv0*/);
        float dU2_DistDir = 0;//QVector3D::dotProduct(dirU2, distG);
        float dV2_DistDir = 0;//QVector3D::dotProduct(dirV2, distG);
        d.setX( 2*(g.x()-h.x())*(gdu.x()) + 2*(g.y()-h.y())*(gdu.y()) + 2*(g.z()-h.z())*(gdu.z()) + 2*DistDir * dU1_DistDir); //du1
        d.setY( 2*(g.x()-h.x())*(gdv.x()) + 2*(g.y()-h.y())*(gdv.y()) + 2*(g.z()-h.z())*(gdv.z()) + 2*DistDir * dV1_DistDir); //dv1
        d.setZ( 2*(g.x()-h.x())*(-hdu.x()) + 2*(g.y()-h.y())*(-hdu.y()) + 2*(g.z()-h.z())*(-hdu.z()) + 2*DistDir * dU2_DistDir); //du2
        d.setW( 2*(g.x()-h.x())*(-hdv.x()) + 2*(g.y()-h.y())*(-hdv.y()) + 2*(g.z()-h.z())*(-hdv.z()) + 2*DistDir * dV2_DistDir); //dv2
        //hessian
        //dx du1
        QVector4D col0;
        col0.setX(2*(g.x()-h.x())*gduu.x() + 2*gdu.x()*gdu.x() + 2*(g.y()-h.y())*gduu.y() + 2*gdu.y()*gdu.y() + 2*(g.z()-h.z())*gduu.z() + 2*gdu.z()*gdu.z() +
                  2*DistDir*/*(QVector3D::dotProduct(dirU1, gdu-gdu0) + QVector3D::dotProduct(dirU1U1, distG) +*/ QVector3D::dotProduct(dir, gduu/*-gduu0*/) /*+ QVector3D::dotProduct(dirU1, gdu-gdu0))*/
                  + 2*dU1_DistDir*dU1_DistDir );
        col0.setY(2*(g.x()-h.x())*gdvu.x() + 2*gdv.x()*gdu.x() + 2*(g.y()-h.y())*gdvu.y() + 2*gdv.y()*gdu.y() + 2*(g.z()-h.z())*gdvu.z() + 2*gdv.z()*gdu.z() +
                  2*DistDir*/*(QVector3D::dotProduct(dirV1, gdu-gdu0) + QVector3D::dotProduct(dirV1U1, distG) +*/ QVector3D::dotProduct(dir, gdvu/*-gdvu0*/) /*+ QVector3D::dotProduct(dirU1, gdv-gdv0))*/
                  + 2*dV1_DistDir*dU1_DistDir );
        col0.setZ(2*gdu.x()*(-hdu.x()) + 2*gdu.y()*(-hdu.y()) + 2*gdu.z()*(-hdu.z()) +
                  /*2*DistDir*(QVector3D::dotProduct(dirU2U1, distG) + QVector3D::dotProduct(dirU2, gdu-gdu0))*/
                  + 2*dU2_DistDir*dU1_DistDir );
        col0.setW(2*gdu.x()*(-hdv.x()) + 2*gdu.y()*(-hdv.y()) + 2*gdu.z()*(-hdv.z()) +
                  /*2*DistDir*(QVector3D::dotProduct(dirV2U1, distG) + QVector3D::dotProduct(dirV2, gdu-gdu0))*/
                  + 2*dV2_DistDir*dU1_DistDir );
        H.setColumn(0, col0);
        //dx dv1
        QVector4D col1;
        col1.setX(2*(g.x()-h.x())*gduv.x() + 2*gdu.x()*gdv.x() + 2*(g.y()-h.y())*gduv.y() + 2*gdu.y()*gdv.y() + 2*(g.z()-h.z())*gduv.z() + 2*gdu.z()*gdv.z() +
                  2*DistDir*/*(QVector3D::dotProduct(dirU1, gdv-gdv0) + QVector3D::dotProduct(dirU1V1, distG) +*/ QVector3D::dotProduct(dir, gduv/*-gduv0*/) /*+ QVector3D::dotProduct(dirV1, gdu-gdu0))*/
                  + 2*dU1_DistDir*dV1_DistDir );
        col1.setY(2*(g.x()-h.x())*gdvv.x() + 2*gdv.x()*gdv.x() + 2*(g.y()-h.y())*gdvv.y() + 2*gdv.y()*gdv.y() + 2*(g.z()-h.z())*gdvv.z() + 2*gdv.z()*gdv.z() +
                  2*DistDir*/*(QVector3D::dotProduct(dirV1, gdv-gdv0) + QVector3D::dotProduct(dirV1V1, distG) +*/ QVector3D::dotProduct(dir, gdvv/*-gdvv0*/) /*+ QVector3D::dotProduct(dirV1, gdv-gdv0))*/
                  + 2*dV1_DistDir*dV1_DistDir );
        col1.setZ(2*gdv.x()*(-hdu.x()) + 2*gdv.y()*(-hdu.y()) + 2*gdv.z()*(-hdu.z()) +
                  /*2*DistDir*(QVector3D::dotProduct(dirU2V1, distG) + QVector3D::dotProduct(dirU2, gdv-gdv0))*/
                  + 2*dU2_DistDir*dV1_DistDir );
        col1.setW(2*gdv.x()*(-hdv.x()) + 2*gdv.y()*(-hdv.y()) + 2*gdv.z()*(-hdv.z()) +
                  /*2*DistDir*(QVector3D::dotProduct(dirV2V1, distG) + QVector3D::dotProduct(dirV2, gdv-gdv0))*/
                  + 2*dV2_DistDir*dV1_DistDir );
        H.setColumn(1, col1);
        //dx du2
        QVector4D col2;
        col2.setX(2*(-hdu.x())*gdu.x() + 2*(-hdu.y())*gdu.y() + 2*(-hdu.z())*gdu.z()
                  /*+ 2*DistDir*(QVector3D::dotProduct(dirU1U2, distG) + QVector3D::dotProduct(dirU2, gdu-gdu0))*/
                  + 2*dU1_DistDir*dU2_DistDir );
        col2.setY(2*(-hdu.x())*gdv.x() + 2*(-hdu.y())*gdv.y() + 2*(-hdu.z())*gdv.z()
                  /*+ 2*DistDir*(QVector3D::dotProduct(dirV1U2, distG) + QVector3D::dotProduct(dirU2, gdv-gdv0))*/
                  + 2*dV1_DistDir*dU2_DistDir );
        col2.setZ(2*(g.x()-h.x())*(-hduu.x()) + 2*(-hdu.x())*(-hdu.x()) + 2*(g.y()-h.y())*(-hduu.y()) + 2*(-hdu.y())*(-hdu.y()) + 2*(g.z()-h.z())*(-hduu.z()) + 2*(-hdu.z())*(-hdu.z())
                  /*+ 2*DistDir*QVector3D::dotProduct(dirU2U2, distG)*/
                  + 2*dU2_DistDir*dU2_DistDir );
        col2.setW(2*(g.x()-h.x())*(-hdvu.x()) + 2*(-hdu.x())*(-hdv.x()) + 2*(g.y()-h.y())*(-hdvu.y()) + 2*(-hdu.y())*(-hdv.y()) + 2*(g.z()-h.z())*(-hdvu.z()) + 2*(-hdu.z())*(-hdv.z())
                  /*+ 2*DistDir*QVector3D::dotProduct(dirV2U2, distG)*/
                  + 2*dV2_DistDir*dU2_DistDir );
        H.setColumn(2, col2);
        //dx dv2
        QVector4D col3;
        col3.setX(2*(-hdv.x())*gdu.x() + 2*(-hdv.y())*gdu.y() + 2*(-hdv.z())*gdu.z()
                  /*+ 2*DistDir*(QVector3D::dotProduct(dirU1V2, distG) + QVector3D::dotProduct(dirV2, gdu-gdu0))*/
                  + 2*dU1_DistDir*dV2_DistDir );
        col3.setY(2*(-hdv.x())*gdv.x() + 2*(-hdv.y())*gdv.y() + 2*(-hdv.z())*gdv.z()
                  /*+ 2*DistDir*(QVector3D::dotProduct(dirV1V2, distG) + QVector3D::dotProduct(dirV2, gdv-gdv0))*/
                  + 2*dV1_DistDir*dV2_DistDir );
        col3.setZ(2*(g.x()-h.x())*(-hduv.x()) + 2*(-hdv.x())*(-hdu.x()) + 2*(g.y()-h.y())*(-hduv.y()) + 2*(-hdv.y())*(-hdu.y()) + 2*(g.z()-h.z())*(-hduv.z()) + 2*(-hdv.z())*(-hdu.z())
                  /*+ 2*DistDir*QVector3D::dotProduct(dirU2V2, distG)*/
                  + 2*dU2_DistDir*dV2_DistDir );
        col3.setW(2*(g.x()-h.x())*(-hdvv.x()) + 2*(-hdv.x())*(-hdv.x()) + 2*(g.y()-h.y())*(-hdvv.y()) + 2*(-hdv.y())*(-hdv.y()) + 2*(g.z()-h.z())*(-hdvv.z()) + 2*(-hdv.z())*(-hdv.z())
                  /*+ 2*DistDir*QVector3D::dotProduct(dirV2V2, distG)*/
                  + 2*dV2_DistDir*dV2_DistDir );
        H.setColumn(3, col3);

        d.setX(turn*(g.x() - h.x()));
        d.setY(turn*(g.y() - h.y()));
        d.setZ(turn*(g.z() - h.z()));
        d.setW(QVector3D::dotProduct(distG, dir) - step);

        // zeros left for clarity
        //QVector4D col0;
        col0.setX(turn*gdu.x() - 0);
        col0.setY(turn*gdu.y() - 0);
        col0.setZ(turn*gdu.z() - 0);
        col0.setW(QVector3D::dotProduct(dirU1, distG) + QVector3D::dotProduct(dir, gdu));
        H.setColumn(0, col0);

        //QVector4D col1;
        col1.setX(turn*gdv.x() - 0);
        col1.setY(turn*gdv.y() - 0);
        col1.setZ(turn*gdv.z() - 0);
        col1.setW(QVector3D::dotProduct(dirV1, distG) + QVector3D::dotProduct(dir, gdv));
        H.setColumn(1, col1);

        //QVector4D col2;
        col2.setX(turn*(0 - hdu.x()));
        col2.setY(turn*(0 - hdu.y()));
        col2.setZ(turn*(0 - hdu.z()));
        col2.setW(QVector3D::dotProduct(dirU2, distG));
        H.setColumn(2, col2);

        //QVector4D col3;
        col3.setX(turn*(0 - hdv.x()));
        col3.setY(turn*(0 - hdv.y()));
        col3.setZ(turn*(0 - hdv.z()));
        col3.setW(QVector3D::dotProduct(dirV2, distG));
        H.setColumn(3, col3);

        H = H.inverted();
        x_new = x - H*d;
        x = x_new;
        stopCond = d.length();//(x_new - x).length();
    //} while (stopCond > e);
    }while(i++ < max_iter);
    return x;
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
