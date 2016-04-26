#include "bsinterpolation.h"

#define SWAP(a,b) {dum=(a);(a)=(b);(b)=dum;}
#define TINY 1.0e-20

int BSInterpolation::id = 0;

BSInterpolation::BSInterpolation()
{
    name = QString("BS interpolation %1").arg(id);
    idname = QString("s%1").arg(id);
    id++;
}

BSInterpolation::BSInterpolation(const QList<Marker *> &m, QMatrix4x4 matrix)
{
    name = QString("BS interpolation %1").arg(id);
    idname = QString("s%1").arg(id);
    id++;

    markers = m;
    InitializeSpline(matrix);
}

void BSInterpolation::InitializeSpline(QMatrix4x4 matrix)
{
    if (markers.length() == 1)
        return;
    if (markers.length() >= 4)
        degree = 3;
    if (markers.length() == 3)
        degree = 2;
    if (markers.length() == 2)
        degree = 1;


    //remove double
    int count = 0;
    for (int i = 0; i<markers.length()-1; i++) {
        if (markers[i]->point.x() == markers[i+1]->point.x() &&
                markers[i]->point.y() == markers[i+1]->point.y() &&
                markers[i]->point.z() == markers[i+1]->point.z()) {
            count++;
        }
    }
    if (count > 0)
        return;

    Clear();
    CalculateParameters();
    CalculateControlPoints();
    InitializeBSpline(matrix);
}

void BSInterpolation::InitializeBSpline(QMatrix4x4 matrix)
{
    if (markers.length() < degree+1)
        return;


    //get points number
    int n = ControlPoints.length();
    float length = 0;
    //initialize Curve
    for (int i = 0; i < n-1; i++) {
        pointsCurve.append(ControlPoints[i].point);
        indicesCurve.append(QPoint(i, i+1));
        QVector4D q1 = Constants::perspectiveMatrix*matrix*ControlPoints[i].point;
        QVector4D q2 = Constants::perspectiveMatrix*matrix*ControlPoints[i+1].point;
        q1 = q1/q1.w();
        q2 = q2/q2.w();
        length += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
    }
    pointsCurve.append(ControlPoints.last().point);

    //algorithm
    int linesNo = (int)length;
    int order = degree+1; //k = degreee+1
    int controlPointsNo = ControlPoints.length(); //n+1
    if(controlPointsNo >= order) {


        for (int i = 0; i<linesNo; i++) { //przedzial otwarty

            float t = ( (float)i/(float)linesNo );// * ( controlPointsNo - (degree) ) + degree;
            //get poin

            QVector4D q;
            for(int i=0; i < controlPointsNo; i++)
            {
                float weightForControl = getBSplineWeight(t, i, degree, knots, parameters.length()-1);
                q += weightForControl * ControlPoints.at(i).point;
            }
            points.append(q);
        }
        for (int i = 0; i<linesNo-1; i++) { //przedzial otwarty
            indices.append(QPoint(i, i+1));
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
    for (int i = 0; i<markers.length()-1; i++) {
        segmentslength.append(sqrt(pow((markers[i+1]->point.x()-markers[i]->point.x()),2)
                                   +pow((markers[i+1]->point.y()-markers[i]->point.y()),2)
                                   +pow((markers[i+1]->point.z()-markers[i]->point.z()),2)));
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

void BSInterpolation::LU(float **a, unsigned long n, int m1, int m2, float **al, unsigned long indx[], float *d)
{
    unsigned long i,j,k,l;
    int mm;
    float dum;

    mm=m1+m2+1;
    l=m1;
    for (i=1;i<=m1;i++) {
        for (j=m1+2-i;j<=mm;j++) a[i][j-l]=a[i][j];
            l--;
        for (j=mm-l;j<=mm;j++) a[i][j]=0.0;
    }
    *d=1.0;
    l=m1;
    for (k=1;k<=n;k++) {
        dum=a[k][1];
        i=k;
        if (l < n) l++;
        for (j=k+1;j<=l;j++) {
            if (fabs(a[j][1]) > fabs(dum)) {
                dum=a[j][1];
                i=j;
            }
        }
        indx[k]=i;
        if (dum == 0.0) a[k][1]=TINY;

        if (i != k) {
            *d = -(*d);
            for (j=1;j<=mm;j++) SWAP(a[k][j],a[i][j]);
        }
        for (i=k+1;i<=l;i++) {
            dum=a[i][1]/a[k][1];
            al[k][i-k]=dum;
            for (j=2;j<=mm;j++) a[i][j-1]=a[i][j]-dum*a[k][j];
            a[i][mm]=0.0;
        }
    }
}

void BSInterpolation::solveEq(float **a, unsigned long n, int m1, int m2, float **al, unsigned long indx[], float b[])
{
    unsigned long i,k,l;
    int mm;
    float dum;

    mm=m1+m2+1;
    l=m1;
    for (k=1;k<=n;k++) {
        i=indx[k];
        if (i != k) SWAP(b[k],b[i])
        if (l < n) l++;
        for (i=k+1;i<=l;i++) b[i] -= al[k][i-k]*b[k];
    }
    l=1;
    for (i=n;i>=1;i--) {
        dum=b[i];
        for (k=2;k<=l;k++) dum -= a[i][k]*b[k+i-1];
        b[i]=dum/a[i][1];
        if (l < mm) l++;
    }
}

void BSInterpolation::initializeDiagonals(float** diagMatrix, int n)
{
    diagMatrix[1][1] = 0;
    diagMatrix[2][1] = 0;
    for(int i = 3; i < n+1; i++)
        diagMatrix[i][1] = getBSplineWeight(parameters[i-1], i-3, degree, knots, n);
    diagMatrix[1][2] = 0;
    for(int i = 2; i < n+1; i++)
        diagMatrix[i][2] = getBSplineWeight(parameters[i-1], i-2, degree, knots, n);
    for(int i = 1; i < n+1; i++)
        diagMatrix[i][3] = getBSplineWeight(parameters[i-1], i-1, degree, knots, n);
    for(int i = 1; i < n; i++)
        diagMatrix[i][4] = getBSplineWeight(parameters[i-1], i, degree, knots, n);
    diagMatrix[n][4] = 0;
    for(int i = 1; i < n-1; i++)
        diagMatrix[i][5] = getBSplineWeight(parameters[i-1], i+1, degree, knots, n);
    diagMatrix[n][5] = 0;
    diagMatrix[n-1][5] = 0;
}

void BSInterpolation::CalculateControlPoints()
{
    const int lowerDiagNo = 2; //m1
    const int upperDiagNo = 2; //m2
    const int diagNo = 5;

    int n = parameters.length();
    float** diagMatrix = (float**)malloc(sizeof(float*) * (n+1));
    float** lowerMatrix = (float**)malloc(sizeof(float*) * (n+1));
    for(int i = 1; i < n+1;i++){
        diagMatrix[i] = (float*)malloc(sizeof(float) * (diagNo+1));
        lowerMatrix[i] = (float*)malloc(sizeof(float) * (lowerDiagNo+1));
    }
    unsigned long* index = (unsigned long*)malloc(sizeof(unsigned long) * (n+1));

    initializeDiagonals(diagMatrix, n);

    float evenOdd = 0;
    LU(diagMatrix, n, lowerDiagNo, upperDiagNo, lowerMatrix, index, &evenOdd);

    //get d table
    float* dx = (float*)malloc(sizeof(float) * (n+1));
    float* dy = (float*)malloc(sizeof(float) * (n+1));
    float* dz = (float*)malloc(sizeof(float) * (n+1));
    for (int i = 1; i<n+1; i++) {
        dx[i] = markers[i-1]->point.x();
        dy[i] = markers[i-1]->point.y();
        dz[i] = markers[i-1]->point.z();
    }

    solveEq(diagMatrix, n , lowerDiagNo, upperDiagNo, lowerMatrix, index, dx);
    solveEq(diagMatrix, n , lowerDiagNo, upperDiagNo, lowerMatrix, index, dy);
    solveEq(diagMatrix, n , lowerDiagNo, upperDiagNo, lowerMatrix, index, dz);
    for (int i = 1; i<n+1; i++) {
        ControlPoints.append(Marker(dx[i], dy[i], dz[i], Qt::gray));
        //markers.append(&ControlPoints[i-1]);
    }
    delete dx;
    delete dy;
    delete dz;
    for(int i = 1; i < n+1; i++){
        delete diagMatrix[i];
        delete lowerMatrix[i];
    }
    delete index;
    delete diagMatrix;
    delete lowerMatrix;

    //3DIAG SOLVER
    /*
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
    */
}

float BSInterpolation::getBSplineWeight(float t, int i, int p, QList<float> knots, int n)
{
    //test if we've reached the bottom of the recursive call
    // When getting the index for the knot function i remember to subtract 1 from i because of the difference caused by us counting from i=1 to n+1 and indexing a vector from 0
    //if out of range

    if (i == n-1 && t == parameters[n-1])
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
    knots.clear();
    parameters.clear();
    ControlPoints.clear();
}

QVector<QPoint> BSInterpolation::getIndices() const
{
    return indices;
}

void BSInterpolation::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> BSInterpolation::getPoints() const
{
    return points;
}

void BSInterpolation::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

QList<Marker *> BSInterpolation::getMarkers() const
{
    return markers;
}

