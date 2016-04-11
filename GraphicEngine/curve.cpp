#include "curve.h"

int Curve::id = 0;

Curve::Curve()
{
    name = QString("curve %1").arg(id);
    idname = QString("b%1").arg(id);
    isBezier = false;
    id++;
}

Curve::Curve(const QList<Marker *> &m, QMatrix4x4 matrix)
{
    name = QString("curve %1").arg(id);
    idname = QString("b%1").arg(id);
    isBezier = false;
    id++;
    markers = m;
    InitializeBezier(matrix);
}

void Curve::InitializeBezier(QMatrix4x4 matrix)
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

void Curve::InitializeBSpline(QMatrix4x4 matrix)
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

QVector4D Curve::getBezierPoint(Segment seg, float t)
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

//i = the weight we're looking for, i should go from 1 to n+1, where n+1 is equal to the total number of control points.
//k = curve order = power/degree +1. eg, to break whole curve into cubics use a curve order of 4
//cps = number of total control points
//t = current step/interp value
float Curve::getBSplineWeight(float t, int i, int k, QList<int> knots)
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

void Curve::ChangeToBezier()
{

}

void Curve::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
    BezierSegments.clear();
}

void Curve::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::green;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

QVector<QPoint> Curve::getIndices() const
{
    return indices;
}

void Curve::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> Curve::getPoints() const
{
    return points;
}

void Curve::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}
