#include "bspline.h"

int BSpline::id = 0;

BSpline::BSpline()
{
    name = QString("bspline %1").arg(id);
    idname = QString("b%1").arg(id);
    id++;
}

BSpline::BSpline(const QList<Marker *> &mark, QMatrix4x4 matrix)
{
    name = QString("bspline %1").arg(id);
    idname = QString("b%1").arg(id);
    id++;
    markers = mark;
    InitializeBSpline(matrix);
}

void BSpline::DrawCurve(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::green;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

void BSpline::InitializeBSpline(QMatrix4x4 matrix)
{
    Clear();
    //get points number
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

    //algorithm
    int linesNo = (int)length;
    int order = 4; //k = degreee+1
    int controlPointsNo = markers.length(); //n+1
    if(controlPointsNo >= order) {
        //initiate knot vector
        int knotSize = order + controlPointsNo;
        QList<int> knots;
        for(int i = 0; i < knotSize; i++)
            knots.append(i);
        //Clear();
        for (int i = 0; i<=linesNo; i++) {
            // use steps to get a 0-1 range value for progression along the curve
            // then get that value into the range [k-1, n+1]
            // k-1 = subCurveOrder-1
            // n+1 = always the number of total control points
            float t = ( (float)i/(float)linesNo ) * ( controlPointsNo - (order-1) ) + order-1;
            //get point
            QVector4D q;
            for(int i=1; i <= controlPointsNo; i++)
            {
                float weightForControl = getBSplineWeight(t, i, order, knots);
                q += weightForControl * markers.at(i-1)->point;
            }
            points.append(q);
        }
        for (int i = 0; i<linesNo; i++) {
            indices.append(QPoint(i, i+1));
        }
    }
}

//i = the weight we're looking for, i should go from 1 to n+1, where n+1 is equal to the total number of control points.
//k = curve order = power/degree +1. eg, to break whole curve into cubics use a curve order of 4
//cps = number of total control points
//t = current step/interp value
float BSpline::getBSplineWeight(float t, int i, int k, QList<int> knots)
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

    float numeratorA = ( t - knots.at(i-1) );
    float denominatorA = ( knots.at(i + k-2) - knots.at(i-1) );
    float numeratorB = ( knots.at(i + k-1) - t );
    float denominatorB = ( knots.at(i + k-1) - knots.at(i) );

    float subweightA = 0;
    float subweightB = 0;

    if( denominatorA != 0 )
        subweightA = numeratorA / denominatorA * getBSplineWeight(t, i, k-1, knots);
    if( denominatorB != 0 )
        subweightB = numeratorB / denominatorB * getBSplineWeight(t, i+1, k-1, knots);

    return subweightA + subweightB;
}

void BSpline::ChangeToBezier()
{

}

void BSpline::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
}

QVector<QPoint> BSpline::getIndices() const
{
    return indices;
}

void BSpline::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> BSpline::getPoints() const
{
    return points;
}

void BSpline::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}
