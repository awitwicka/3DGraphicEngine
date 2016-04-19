#include "bsinterpolation.h"

int BSInterpolation::id = 0;

BSInterpolation::BSInterpolation()
{

}

BSInterpolation::BSInterpolation(const QList<Marker *> &m, QMatrix4x4 matrix)
{
    name = QString("BS interpolation %1").arg(id);
    idname = QString("i%1").arg(id);
    id++;

    DataPoints = m;
    CalculateParameters();
    CalculateControlPoints();
    InitializeBezier(matrix);
}

void BSInterpolation::InitializeBezier(QMatrix4x4 matrix)
{
    Clear();
    int n = markers.length();
    float length = 0;
    //initialize Curve
    for (int i = 0; i < n-1; i++) {
        pointsCurve.append(markers[i]->point);
        indicesCurve.append(QPoint(i, i+1));
        QVector4D q1 = Constants::perspectiveMatrix*matrix*markers[i]->point;
        QVector4D q2 = Constants::perspectiveMatrix*matrix*markers[i+1]->point;
        q1 = q1/q1.w();
        q2 = q2/q2.w();
        length += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
    }
    pointsCurve.append(markers.last()->point);
    //divide on as many curves of the 3th deg: ex: 4 3 3 3.... 3 2/1

    if (n>0) {
        QList<Marker*> m;
        for (int i = 1; i < n; i) {
            m.append(markers[i-1]);
            for (int c = 0; c<3; c++) {
                if (i<n)
                    m.append(markers[i]);
                i++;
            }
            BezierSegments.append(Segment(m));
            m.clear();
        }
    }

    //Clear() todo: move to cadobject
    int linesNo = (int)length;
    int j=0;
    for (int s = 0; s<BezierSegments.length(); s++) {
        for (int i = 0; i <= linesNo; i++) {
            points.append(getBezierPoint(BezierSegments[s], (float)i/(float)linesNo));
        }
        for (j; j < (linesNo*(s+1)); j++) {
            indices.append(QPoint(j, j+1));
        }
    }
}

QVector4D BSInterpolation::getBezierPoint(Segment seg, float t)
{
    QVector4D* tmp = new QVector4D[seg.markers.length()];
    for (int i = 0; i < seg.pointsNo; i++) {
        tmp[i] = seg.markers[i]->point;
    }
    int degree = seg.pointsNo-1;
    while (degree > 0) {
        for (int k = 0; k < degree; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        degree--;
    }
    //todo w = 1;
    QVector4D p = tmp[0];
    delete[] tmp;
    return p;
}


void BSInterpolation::CalculateParameters()
{
    //int knotsNo;
    //calculate length between segments
    QList<float> segmentslength; //0...n+1
    float totalLength = 0;
    for (int i = 0; i<DataPoints.length()-1; i++) {
        segmentslength.append(sqrt(pow((DataPoints[i+1]->point.x()-DataPoints[i]->point.x()),2)
                                   +pow((DataPoints[i+1]->point.y()-DataPoints[i]->point.y()),2)
                                   +pow((DataPoints[i+1]->point.z()-DataPoints[i]->point.z()),2)));
        totalLength += segmentslength[i];
    }
    //data set -> n+1 length
    //calculate parameters t -> length n+1
    parameters.append(0);
    float sum = 0;
    for (int i = 0; i<segmentslength.length(); i++) {
        sum += segmentslength[i];
        parameters.append(sum /totalLength);
    }
    //calculate knots -> n+1+p+1 = m+1 length
    //first and last p+1 point -> 0 and 1 respectively if clamped
    //knots - avarage of p parameters
    for (int i = 0; i<degree+1; i++) {
        knots.append(0);
    }
    sum = 0;
    for (int i = 1; i<parameters.length()-degree; i++) { //i = 1... n-p
        for (int j = i; j<degree+i; j++)
            sum += parameters[j];
        knots.append(sum/(float)degree);
        sum = 0;
    }
    for (int i = 0; i<degree+1; i++) {
        knots.append(1);
    }
}

void BSInterpolation::CalculateControlPoints()
{
    //assume degree 3
    int p = 3;
    //basis functions
    int n1 = parameters.length();
    QList<float> a; //low diag  a
    QList<float> b; //middle diag   b
    QList<float> c; //upper diag   c
    QList<float> c_original;

    a.append(0);
    b.append(getBSplineWeight(parameters[0], 0, p, knots, n1-1)); //Ni{ti}
    c_original.append(getBSplineWeight(parameters[0], 1, p, knots, n1-1)); //Ni+1{ti}
    for (int i = 1; i<n1-1; i++) {
        b.append(getBSplineWeight(parameters[i], i, p, knots, n1-1)); //Ni{ti}
        a.append(getBSplineWeight(parameters[i], i-1, p, knots, n1-1)); //Ni-1{ti}
        c_original.append(getBSplineWeight(parameters[i], i+1, p, knots, n1-1)); //Ni+1{ti}
    }
    b.append(getBSplineWeight(parameters[n1-1], n1-1, p, knots, n1-1)); //Ni{ti}
    a.append(getBSplineWeight(parameters[n1-1], n1-2, p, knots, n1-1)); //Ni-1{ti}
    c_original.append(0);

    //get d table
    QList<float> dx; //datapoints.x
    QList<float> dy;
    QList<float> dz;
    for (int i = 0; i<DataPoints.length(); i++) {
        dx.append(DataPoints[i]->point.x());
        dy.append(DataPoints[i]->point.y());
        dz.append(DataPoints[i]->point.z());
    }

    //SOLVE D=N*P to get P - control points
    //dx dy dz
    c = c_original;
    int n = n1-1;
    c[0] /= b[0];
    dx[0] /= b[0];
    dy[0] /= b[0];
    dz[0] /= b[0];
    for (int i = 1; i < n; i++) {
        c[i] /= b[i] - a[i]*c[i-1];
        dx[i] = (dx[i] - a[i]*dx[i-1]) / (b[i] - a[i]*c[i-1]);
        dy[i] = (dy[i] - a[i]*dy[i-1]) / (b[i] - a[i]*c[i-1]);
        dz[i] = (dz[i] - a[i]*dz[i-1]) / (b[i] - a[i]*c[i-1]);
    }
    dx[n] = (dx[n] - a[n]*dx[n-1]) / (b[n] - a[n]*c[n-1]);
    dy[n] = (dy[n] - a[n]*dy[n-1]) / (b[n] - a[n]*c[n-1]);
    dz[n] = (dz[n] - a[n]*dz[n-1]) / (b[n] - a[n]*c[n-1]);

    for (int i = n; i-- > 0;) {
        dx[i] -= c[i]*dx[i+1];
        dy[i] -= c[i]*dy[i+1];
        dz[i] -= c[i]*dz[i+1];
    }
    for (int i = 0; i<DataPoints.length(); i++) {
        ControlPoints.append(Marker(dx[i], dy[i], dz[i]));
        markers.append(&ControlPoints[i]);
    }
}

//k = p+1 - degree+1
//i - i+1 starting at 1
float BSInterpolation::getBSplineWeight(float t, int i, int p, QList<float> knots, int n)
{
    //test if we've reached the bottom of the recursive call
    // When getting the index for the knot function i remember to subtract 1 from i because of the difference caused by us counting from i=1 to n+1 and indexing a vector from 0

    if (i == n && t == parameters[n])
        return 1;
    if (i == 0 && t == parameters[0])
        return 1;

    if( p == 0 )
    {
        if( t >= knots.at(i) && t < knots.at(i+1) )
            return 1;
        else
            return 0;
    }

    float d1 = ( knots.at(i + p) - knots.at(i) );
    float d2 = ( knots.at(i + p + 1) - knots.at(i + 1) );

    float N1 = 0;
    float N2 = 0;

    if( d1 != 0 )
        N1 = ( t - knots.at(i) ) / d1 * getBSplineWeight(t, i, p-1, knots, n);
    if( d2 != 0 )
        N2 = ( knots.at(i + p + 1) - t ) / d2 * getBSplineWeight(t, i+1, p-1, knots, n);

    return N1 + N2;
}

void BSInterpolation::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
    BezierSegments.clear();
}
