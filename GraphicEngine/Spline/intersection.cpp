#include "intersection.h"

int Intersection::id = 0;

Intersection::Intersection()
{

}

Intersection::Intersection(QMatrix4x4 matrix, Marker* start, CADSplinePatch *patch1, CADSplinePatch *patch2, float step)
{
    name = QString("Intersection%1").arg(id);
    idname = QString("x%1").arg(id);
    id++;

    series1 = new QLineSeries();
    series2 = new QLineSeries();

    chart1 = new QChart();
    chart1->legend()->hide();
    chart1->addSeries(series1);
    chart1->createDefaultAxes();
    chart1->setTitle("UV patch1");
    chart2 = new QChart();
    chart2->legend()->hide();
    chart2->addSeries(series2);
    chart2->createDefaultAxes();
    chart2->setTitle("UV patch2");

    chartView1 = new QChartView(chart1);
    chartView1->setRenderHint(QPainter::Antialiasing);
    chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing);

    window1.setCentralWidget(chartView1);
    window1.resize(400, 400);
    window2.setCentralWidget(chartView2);
    window2.resize(400, 400);

    this->patch1 = patch1;
    this->patch2 = patch2;
    this->step = step;

    Color = Qt::red;
    CalculateIntersection(patch2, start, patch1);
}

Intersection::~Intersection()
{
    delete series1;
    delete series2;
    delete chart1;
    delete chart2;
    delete chartView1;
    delete chartView2;
}

void Intersection::CalculateIntersection(CADSplinePatch *patch2, Marker* start, CADSplinePatch *patch1)
{
    //step accuracy
    double e = 0.00000001;
    double a = 0.0005; //step'
    double epsilon = 0.04f;
    int count1 = 0;
    int count2 = 0;

    UVPointData point1 = FindClosesPointOnSurface(start->point, patch1, 0.01);
    UVPointData point2 = FindClosesPointOnSurface(start->point, patch2, 0.01);

    //show closest point on the surface
    pointsCurve.append(start->point);
    pointsCurve.append(point1.position);
    pointsCurve.append(start->point);
    pointsCurve.append(point2.position);
    indicesCurve.append(QPoint(0, 1));
    indicesCurve.append(QPoint(2, 3));


    //Gradient descend method - finding first intersection point
    QVector4D x = QVector4D(point1.u, point1.v, point2.u, point2.v); //start point u1v1u2v2
    QVector4D x_optimal = GradientDistanceMinimalization(e, a, x, patch1, patch2);


    int iter = 0;
    const int MAX_ITER = 1500;
    QVector4D point = x_optimal;
    QVector4D nextStep;
    do{
        //nextStep = GradientStep(e, a, point, patch1, patch2);
        //point = GradientDistanceMinimalization(e, a, nextStep, patch1, patch2);
        point = NewtonNextPoint(e, point, patch1, patch2);
        //point = GradientNextIntersection(e, a, point, patch1, patch2);


        if (turn == 1) {
            UVparameters.push_back(point);
        } else if(turn == -1) {
            UVparameters.push_front(point);
        }

        //TODO: distance instead of checking separately x and y (step instead of e)
        //TODO: check whats wrong with patch kolejność
        //going around cylinder

        if (!patch1->isPlane) {
            if (point.x() < 0.0f) {
                point.setX(point.x()+1.0f);
                count1++;
            }
            else if(point.x() > 1.0f) {
                point.setX(point.x()-1.0f);
                count1++;
            }
            //check if made a full circle
            //if (turn == 1 && count1>0 && fabs(x_optimal.x()-point.x())<epsilon && fabs(x_optimal.y()-point.y())<epsilon && iter>10) //add direction/gradient check
            //    turn = 0;
        }
        else if (!patch2->isPlane) {
            if (point.z() < 0.0f) {
                point.setZ(point.z()+1.0f);
                count2++;
            }
            else if(point.z() > 1.0f) {
                point.setZ(point.z()-1.0f);
                count2++;
            }
            //check if made a full circle
            //if (turn == 1 && count2>0 && fabs(x_optimal.z()-point.z())<e && fabs(x_optimal.w()-point.w())<e && iter>10) //add direction/gradient check
            //    turn = 0;
        }

        //change turn status
        if((point.x() < 0 || point.y() < 0 || point.z() < 0 || point.w() < 0)
                || (point.x() > 1.0f || point.y() > 1.0f || point.z() > 1.0f || point.w() > 1.0f)
                || (point.x()!= point.x() || point.y()!= point.y() || point.z()!= point.z() || point.w()!= point.w() )) {

            //TODO: if cone then check also on both sides of its 0 point
            if (turn == 1) {
                turn = -1;
                point = UVparameters.first(); //TODO correct first dist minimalisation
            }
            else if (turn == -1)
                turn = 0; // end loop
        }
        iter++;
   }while(turn != 0 && iter < MAX_ITER);

    //draw intersection
    int count = 0;
    for (int i = 0; i<UVparameters.length(); i++) {
        points.append((patch1->ComputePos(UVparameters[i].x(), UVparameters[i].y())
                       + patch2->ComputePos(UVparameters[i].z(), UVparameters[i].w()))/2);
        indices.append(QPoint(count, count+1));
        series1->append(UVparameters[i].x(), UVparameters[i].y());
        series2->append(UVparameters[i].z(), UVparameters[i].w());
        count++;
    }
    indices.removeLast();
    //*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    window1.show();
    window2.show();
}

QVector4D Intersection::GradientDistanceMinimalization(double e, double a, QVector4D x, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    //function f(u1v1u2v2) = sqrt((g.x-h.x)^2 + (g.y-h.y)^2 + (g.z-h.z)^2)
    //subfunctions g(u1v1) - patch1->ComputePos(u, v);
    //subfunctions h(u2v2) - patch2->ComputePos(u, v);
    //gradient d = 2*(g.x-h.x)*(g.x'-h.x') + 2*(g.y-h.y)*(g.y'-h.y') + 2*(g.z-h.z)*(g.z'-h.z') /
    //             / (2*f(u1v1u2v2))

    //int count = indicesCurve.length()/2;
    QVector4D new_x = x;
    double stopCond1;
    double stopCond2;
    double new_f;
    do {
        //pointsCurve.append(patch1->ComputePos(x.x(), x.y()));
        //indicesCurve.append(QPoint(count, count+1));
        //count +=1;

        //get f
        QVector4D g = patch1->ComputePos(x.x(), x.y()); //u1 v1
        QVector4D h = patch2->ComputePos(x.z(), x.w()); //u2 v2
        double f = sqrt(pow((g.x()-h.x()),2) + pow((g.y()-h.y()),2) + pow((g.z()-h.z()),2)); // u1 v1 u2 v2 - distance between 2 points
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
        QVector4D new_d;
        new_d.setX( (2*(new_g.x()-new_h.x())*(gdu.x()) + 2*(new_g.y()-new_h.y())*(gdu.y()) + 2*(new_g.z()-new_h.z())*(gdu.z())) / (2*f) ); //du1
        new_d.setY( (2*(new_g.x()-new_h.x())*(gdv.x()) + 2*(new_g.y()-new_h.y())*(gdv.y()) + 2*(new_g.z()-new_h.z())*(gdv.z())) / (2*f) ); //dv1
        new_d.setZ( (2*(new_g.x()-new_h.x())*(-hdu.x()) + 2*(new_g.y()-new_h.y())*(-hdu.y()) + 2*(new_g.z()-new_h.z())*(-hdu.z())) / (2*f) ); //du2
        new_d.setW( (2*(new_g.x()-new_h.x())*(-hdv.x()) + 2*(new_g.y()-new_h.y())*(-hdv.y()) + 2*(new_g.z()-new_h.z())*(-hdv.z())) / (2*f) ); //dv2
        new_f = sqrt(pow((new_g.x()-new_h.x()),2) + pow((new_g.y()-new_h.y()),2) + pow((new_g.z()-new_h.z()),2));

        stopCond1 = (new_x - x).length();
        stopCond2 = new_d.lengthSquared();
        if (new_f >= f) a /= 2;
        else x = new_x;
     } while(stopCond1 > e && stopCond2 > e); //is length already abs? - pkt stabilny
    //pointsCurve.append(patch1->ComputePos(x.x(), x.y()));
    return new_x;
}

QVector4D Intersection::GradientStep(double e, double a, QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    double new_f;
    double stopCond;

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

        double f = pow((QVector3D::dotProduct(g-g0, dir) - step),2) + pow((QVector3D::dotProduct(h-h0, dir) - step),2);
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

QVector4D Intersection::GradientNextIntersection(double e, double a, QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    double new_f;
    double stopCond;
    QVector4D d; //gradinet of the function
    QVector3D dir = GetDirection(startPoint, patch1, patch2);

    QVector3D g0 = QVector3D(patch1->ComputePos(startPoint.x(), startPoint.y())); //u1 v1
    QVector3D h0 = QVector3D(patch2->ComputePos(startPoint.z(), startPoint.w())); //u2 v2

    QVector4D x = startPoint;
    QVector4D best_x = x;
    QVector4D new_x = x;
    do {
        QVector3D g = QVector3D(patch1->ComputePos(best_x.x(), best_x.y())); //u1 v1
        QVector3D h = QVector3D(patch2->ComputePos(best_x.z(), best_x.w())); //u2 v2

        float distDirG = QVector3D::dotProduct(g-g0, dir) - step;
        float distDirH = QVector3D::dotProduct(h-h0, dir) - step;
        double f = GoalFunction(g, h, distDirG, distDirH);
        SetGoalFunctionDerivative(best_x, patch1, patch2, d, distDirG, distDirH, dir);

        //TODO: minimalizacja kierunkowa
        //find a such that f(x - (a*d)) is mininum

        new_x = x - (a*d);
        QVector3D new_g = QVector3D(patch1->ComputePos(new_x.x(), new_x.y())); //u1 v1
        QVector3D new_h = QVector3D(patch2->ComputePos(new_x.z(), new_x.w())); //u2 v2
        new_f = GoalFunction(new_g, new_h, QVector3D::dotProduct(new_g-g0, dir)-step, QVector3D::dotProduct(new_h-h0, dir)-step);


        stopCond = (new_x - x).length();
        if (new_f >= f)
            a /= 2;
        else
            best_x = new_x;

        x = new_x;
    } while(stopCond > e); //is length already abs? - pkt stabilny
    return best_x;
}

Intersection::UVPointData Intersection::FindClosesPointOnSurface(QVector4D PointPos, CADSplinePatch *patch, double accuracy)
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
           //pointsCurve.append(tmpPos);
           //indicesCurve.append(QPoint(count, count+1));
           //count +=1;
        }
    }
    //pointsCurve.append(tmpPos);
    return UVPointData(pos, Uval, Vval);
}

QVector4D Intersection::NewtonNextPoint(double e, QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2) //u1v1u2v2 point
{
    double stopCond;
    int max_iter = 10;
    int i = 0;

    QVector4D x_new;
    QMatrix4x4 H; //Hessian matrix
    QVector4D d; //gradinet of the function
    QVector3D dir = GetDirection(startPoint, patch1, patch2);

    //g0 start
    QVector3D g0 = QVector3D(patch1->ComputePos(startPoint.x(), startPoint.y())); //u1 v1
    QVector3D h0 = QVector3D(patch2->ComputePos(startPoint.z(), startPoint.w())); //u2 v2

    QVector3D dirU1 = QVector3D(0,0,0);
    QVector3D dirV1 = QVector3D(0,0,0);
    QVector3D dirU2 = QVector3D(0,0,0);
    QVector3D dirV2 = QVector3D(0,0,0);

    QVector4D x = startPoint;
    do{
        QVector3D g = QVector3D(patch1->ComputePos(x.x(), x.y())); //u1 v1
        QVector3D gdu = QVector3D(patch1->ComputeDu(x.x(), x.y())); //u1' v1
        QVector3D gdv = QVector3D(patch1->ComputeDv(x.x(), x.y())); //u1 v1'

        QVector3D h = QVector3D(patch2->ComputePos(x.z(), x.w())); //u2 v2
        QVector3D hdu = QVector3D(patch2->ComputeDu(x.z(), x.w())); //u2' v2
        QVector3D hdv = QVector3D(patch2->ComputeDv(x.z(), x.w())); //u2 v2'

        QVector3D distG = g-g0;
        double DistDir = QVector3D::dotProduct(distG, dir) - step;
        double f = pow((g.x()-h.x()),2) + pow((g.y()-h.y()),2) + pow((g.z()-h.z()),2) + pow(DistDir,2); // u1 v1 u2 v2 - distance between 2 points

        d.setX(turn*(g.x() - h.x()));
        d.setY(turn*(g.y() - h.y()));
        d.setZ(turn*(g.z() - h.z()));
        d.setW(QVector3D::dotProduct(distG, dir) - step);

        QVector4D col0;
        col0.setX(turn*gdu.x() - 0);
        col0.setY(turn*gdu.y() - 0);
        col0.setZ(turn*gdu.z() - 0);
        col0.setW(QVector3D::dotProduct(dirU1, distG) + QVector3D::dotProduct(dir, gdu));
        H.setColumn(0, col0);

        QVector4D col1;
        col1.setX(turn*gdv.x() - 0);
        col1.setY(turn*gdv.y() - 0);
        col1.setZ(turn*gdv.z() - 0);
        col1.setW(QVector3D::dotProduct(dirV1, distG) + QVector3D::dotProduct(dir, gdv));
        H.setColumn(1, col1);

        QVector4D col2;
        col2.setX(turn*(0 - hdu.x()));
        col2.setY(turn*(0 - hdu.y()));
        col2.setZ(turn*(0 - hdu.z()));
        col2.setW(QVector3D::dotProduct(dirU2, distG));
        H.setColumn(2, col2);

        QVector4D col3;
        col3.setX(turn*(0 - hdv.x()));
        col3.setY(turn*(0 - hdv.y()));
        col3.setZ(turn*(0 - hdv.z()));
        col3.setW(QVector3D::dotProduct(dirV2, distG));
        H.setColumn(3, col3);

        H = H.inverted();
        x_new = x - H*d;
        x = x_new;
        stopCond = (x_new - x).length();
    } while (stopCond > e);
    //}while(i++ < max_iter);
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

double Intersection::GoalFunction(QVector3D g, QVector3D h, double DistDirG, double DistDirH)
{
    double f = pow((g.x()-h.x()),2) + pow((g.y()-h.y()),2) + pow((g.z()-h.z()),2)
            + pow(DistDirG,2) + pow(DistDirH,2);
    return f;
}

void Intersection::SetGoalFunctionDerivative(QVector4D best_x, CADSplinePatch *patch1, CADSplinePatch *patch2, QVector4D &d, double DistDirG, double DistDirH, QVector3D dir)
{
    QVector3D g = QVector3D(patch1->ComputePos(best_x.x(), best_x.y())); //u1 v1
    QVector3D h = QVector3D(patch2->ComputePos(best_x.z(), best_x.w())); //u2 v2
    QVector3D gdu = QVector3D(patch1->ComputeDu(best_x.x(), best_x.y())); //u1' v1
    QVector3D gdv = QVector3D(patch1->ComputeDv(best_x.x(), best_x.y())); //u1 v1'
    QVector3D hdu = QVector3D(patch2->ComputeDu(best_x.z(), best_x.w())); //u2' v2
    QVector3D hdv = QVector3D(patch2->ComputeDv(best_x.z(), best_x.w())); //u2 v2'

    d.setX( (2*(g.x()-h.x())*(gdu.x())  + 2*(g.y()-h.y())*(gdu.y())  + 2*(g.z()-h.z())*(gdu.z()))   + 2 * DistDirG * QVector3D::dotProduct(dir, gdu)); //du1
    d.setY( (2*(g.x()-h.x())*(gdv.x())  + 2*(g.y()-h.y())*(gdv.y())  + 2*(g.z()-h.z())*(gdv.z()))   + 2 * DistDirG * QVector3D::dotProduct(dir, gdv)); //dv1
    d.setZ( (2*(g.x()-h.x())*(-hdu.x()) + 2*(g.y()-h.y())*(-hdu.y()) + 2*(g.z()-h.z())*(-hdu.z()))  + 2 * DistDirH * QVector3D::dotProduct(dir, hdu)); //du2
    d.setW( (2*(g.x()-h.x())*(-hdv.x()) + 2*(g.y()-h.y())*(-hdv.y()) + 2*(g.z()-h.z())*(-hdv.z()))  + 2 * DistDirH * QVector3D::dotProduct(dir, hdv)); //dv2
}

QVector3D Intersection::GetDirection(QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    QVector3D gdu0 = QVector3D(patch1->ComputeDu(startPoint.x(), startPoint.y())); //u1' v1
    QVector3D gdv0 = QVector3D(patch1->ComputeDv(startPoint.x(), startPoint.y())); //u1 v1'

    QVector3D hdu0 = QVector3D(patch2->ComputeDu(startPoint.z(), startPoint.w())); //u2' v2
    QVector3D hdv0 = QVector3D(patch2->ComputeDv(startPoint.z(), startPoint.w())); //u2 v2'

    //normal to the surface
    QVector3D Ng = QVector3D::crossProduct(gdu0, gdv0);
    QVector3D Nh = QVector3D::crossProduct(hdu0, hdv0);
    //searching direction
    QVector3D dir = (QVector3D::crossProduct(Ng, Nh)).normalized();
    dir *= turn;
    return dir;
}
