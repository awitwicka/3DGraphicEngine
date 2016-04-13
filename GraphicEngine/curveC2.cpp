#include "curveC2.h"

int CurveC2::id = 0;

CurveC2::CurveC2()
{
    name = QString("curveC2 %1").arg(id);
    idname = QString("b%1").arg(id);
    isBezier = false;
    degree = 3;
    id++;
}

CurveC2::CurveC2(const QList<Marker *> &m, QMatrix4x4 matrix)
{
    name = QString("curveC2 %1").arg(id);
    idname = QString("b%1").arg(id);
    isBezier = false;
    degree = 3;
    id++;
    boorMarkers = m;
    InitializeBSpline(matrix);
}

void CurveC2::InitializeBezierC2(QMatrix4x4 matrix)
{
    //WORKS ONLY FOR CUBIC BSPLINES!!!!
    if (boorMarkers.length() < degree+1)
        return;

    Clear();
    int n = bezierMarkers.length();
    float length = 0;
    //initialize Curve
    for (int i = 0; i < n-1; i++) {
        pointsCurve.append(bezierMarkers[i].point);
        indicesCurve.append(QPoint(i, i+1));
        QVector4D q1 = Constants::perspectiveMatrix*matrix*bezierMarkers[i].point;
        QVector4D q2 = Constants::perspectiveMatrix*matrix*bezierMarkers[i+1].point;
        q1 = q1/q1.w();
        q2 = q2/q2.w();
        length += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
    }
    pointsCurve.append(bezierMarkers.last().point);
    //divide on as many curves of the 3th deg: ex: 4 3 3 3.... 3 2/1

    if (n>0) {
        QList<Marker*> m;
        for (int i = 1; i < n; i) {
            m.append(&bezierMarkers[i-1]);
            for (int c = 0; c<3; c++) {
                if (i<n)
                    m.append(&bezierMarkers[i]);
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

void CurveC2::InitializeBSpline(QMatrix4x4 matrix)
{   
    Clear();
    //get points number
    int n = boorMarkers.length();
    float length = 0;
    //initialize Curve
    for (int i = 0; i < n-1; i++) {
        pointsCurve.append(boorMarkers[i]->point);
        indicesCurve.append(QPoint(i, i+1));
        QVector4D q1 = Constants::perspectiveMatrix*matrix*boorMarkers[i]->point;
        QVector4D q2 = Constants::perspectiveMatrix*matrix*boorMarkers[i+1]->point;
        q1 = q1/q1.w();
        q2 = q2/q2.w();
        length += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
    }
    pointsCurve.append(boorMarkers.last()->point);

    //algorithm
    int linesNo = (int)length;
    int order = degree+1; //k = degreee+1
    int controlPointsNo = boorMarkers.length(); //n+1
    if(controlPointsNo >= order) {
        //initiate knot vector
        int knotSize = order + controlPointsNo;
        QList<int> knots; //TODO Vector
        for(int i = 0; i < knotSize; i++)
            knots.append(i);
        //Clear();
        for (int i = 0; i<=linesNo; i++) {
            // use steps to get a 0-1 range value for progression along the curve
            // then get that value into the range [k-1, n+1]
            // k-1 = subCurveOrder-1
            // n+1 = always the number of total control points
            float t = ( (float)i/(float)linesNo ) * ( controlPointsNo - (order-1) ) + order-1;
            //get poin

            QVector4D q;
            for(int i=1; i <= controlPointsNo; i++)
            {
                float weightForControl = getBSplineWeight(t, i, order, knots);
                q += weightForControl * boorMarkers.at(i-1)->point;
            }
            points.append(q);
        }
        for (int i = 0; i<linesNo; i++) {
            indices.append(QPoint(i, i+1));
        }
    }
}

QVector4D CurveC2::getBezierPoint(Segment seg, float t)
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
float CurveC2::getBSplineWeight(float t, int i, int k, QList<int> knots)
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

void CurveC2::ChangeToBezier()
{
    //WORKS ONLY FOR CUBIC BSPLINES!!!!
    if (boorMarkers.length() < degree+1)
        return;
    int SegmentsNo = (boorMarkers.length()-1) - (degree-1);//no odcinkow - (degree-1)
    bezierMarkers.clear();
    //level1
    QList<VirtualMarkInfo> level1;
    for (int i = 0; i < boorMarkers.length()-1; i++) {
        QVector4D q1 = boorMarkers[i]->point;
        QVector4D q2 = boorMarkers[i+1]->point;

        VirtualMarkInfo v13;
        v13.position = q1 + (1.0f/3.0f)*(q2 - q1);
        v13.parent = boorMarkers[i];
        v13.partner = boorMarkers[i+1];
        level1.append(v13);

        VirtualMarkInfo v23;
        v23.position = q1 + (2.0f/3.0f)*(q2 - q1);
        v23.parent = boorMarkers[i+1];
        v23.partner = boorMarkers[i];
        level1.append(v23);
    }
    //level2
    QList<VirtualMarkInfo> level2;
    for (int i = 0; i<boorMarkers.length()-2; i++) {
        QVector4D q1 = level1[i*2+1].position;
        QVector4D q2 = level1[(i+1)*2].position;

        VirtualMarkInfo v12;
        v12.position = q1 + 0.5f*(q2 - q1);
        v12.parent = boorMarkers[i+1];
        v12.partner = nullptr;
        level2.append(v12);
    }
    //bezier
    for (int i = 0; i <SegmentsNo; i++) {
        bezierMarkers.append(Marker(level2[i].position, Qt::gray, level2[i].parent, level2[i].partner, this));
        bezierMarkers.append(Marker(level1[(i+1)*2].position, Qt::gray, level1[(i+1)*2].parent, level1[(i+1)*2].partner, this));
        bezierMarkers.append(Marker(level1[(i+1)*2+1].position, Qt::gray, level1[(i+1)*2+1].parent, level1[(i+1)*2+1].partner, this));
    }
    bezierMarkers.append(Marker(level2.back().position, Qt::gray, level2.back().parent, level2.back().partner, this));
    /*if (boorMarkers.length() < degree+1)
        return;
    int SegmentsNo = (boorMarkers.length()-1) - (degree-1);//no odcinkow - (degree-1)
    bezierMarkers.clear();
    //level1
    QList<QVector4D> level1;
    for (int i = 0; i<boorMarkers.length()-1; i++) {
        QVector4D q1 = boorMarkers[i]->point;
        QVector4D q2 = boorMarkers[i+1]->point;
        QVector4D q13 = q1 + (1.0f/3.0f)*(q2 - q1);
        QVector4D q23 = q1 + (2.0f/3.0f)*(q2 - q1);
        level1.append(q13);
        level1.append(q23);
    }
    //level2
    QList<QVector4D> level2;
    for (int i = 0; i<boorMarkers.length()-2; i++) {
        QVector4D q1 = level1[i*2+1];
        QVector4D q2 = level1[(i+1)*2];
        QVector4D q12 = q1 + 0.5f*(q2 - q1);
        level2.append(q12);
    }
    //bezier
    for (int i = 0; i <SegmentsNo; i++) {
        bezierMarkers.append(Marker(level2[i], Qt::gray));
        bezierMarkers.append(Marker(level1[(i+1)*2], Qt::gray));
        bezierMarkers.append(Marker(level1[(i+1)*2+1], Qt::gray));
    }
    bezierMarkers.append(Marker(level2.back(), Qt::gray));*/
}

void CurveC2::AdjustOtherPoints(Marker* m, QVector4D oldPosition)
{
    m->boorParent->point = m->boorParent->point + (3.0/2.0)*(m->point - oldPosition);
    //ChangeToBezier();
}

void CurveC2::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
    BezierSegments.clear();
}

void CurveC2::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::green;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

QVector<QPoint> CurveC2::getIndices() const
{
    return indices;
}

void CurveC2::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> CurveC2::getPoints() const
{
    return points;
}

void CurveC2::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}
