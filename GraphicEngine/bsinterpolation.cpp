#include "bsinterpolation.h"

BSInterpolation::BSInterpolation()
{

}

BSInterpolation::BSInterpolation(const QList<Marker *> &m, QMatrix4x4 matrix)
{

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
        totalLength += segmentslength[0];
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
    float sum = 0;
    for (int i = 1; i<parameters.length()-degree; i++) { //i = 1... n-p
        for (int j = i; j<degree+i; j++)
            sum += parameters[j];
        knots.append(sum/degree);
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
    QMatrix4x4 N = QMatrix4x4(0,n1,n1);
    for (int r = 0; r<n1; r++) {
        for (int i=-1; i<p+1; i++) {
            QVector4D n1 = getBSplineWeight(parameters[0], i+1, p+1, knots);
            N.row(0) = ni;
       }
    }
}

//k = p+1 - degree+1
//i - i+1 starting at 1
float BSInterpolation::getBSplineWeight(float t, int i, int k, QList<float> knots)
{
    //test if we've reached the bottom of the recursive call
    // When getting the index for the knot function i remember to subtract 1 from i because of the difference caused by us counting from i=1 to n+1 and indexing a vector from 0
    if( k == 1 )
    {
        if( t >= knots.at(i-1) && t < knots.at(i) )
            return 1;
        else
            return 0;
    }

    float d1 = ( knots.at(i + k-2) - knots.at(i-1) );
    float d2 = ( knots.at(i + k-1) - knots.at(i) );

    float N1 = 0;
    float N2 = 0;

    if( d1 != 0 )
        N1 = ( t - knots.at(i-1) ) / d1 * getBSplineWeight(t, i, k-1, knots);
    if( d2 != 0 )
        N2 = ( knots.at(i + k-1) - t ) / d2 * getBSplineWeight(t, i+1, k-1, knots);

    return N1 + N2;
}

