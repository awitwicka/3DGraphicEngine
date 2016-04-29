#include "bicubicsegment.h"

BicubicSegment::BicubicSegment()
{

}

BicubicSegment::BicubicSegment(QList<Marker *> m, int u, int v, float length)
{
    if (m.length() != ORDER*ORDER)
        return;
    U = u;
    V = v;
    segLength = length;
    for (int y = 0; y<ORDER; y++) {
        for (int x = 0; x<ORDER; x++) {
            markers[x][y] = m[y*ORDER + x];
        }
    }
}

void BicubicSegment::InitializeSpline(QMatrix4x4 matrix)
{
    float unitX = segLength/U;
    float unitY = segLength/V;
    Clear();



    int linesNo = 100;//(int)length;
    for (int u = 0; u < U+1; u++) {
        //calculate points
        QVector4D data[ORDER];
        for (int i = 0; i<ORDER; i++)
           data[i] = getBezierPointRow(i, (u*unitX)/(unitX*U));
        //get bezier line from calculated points
        for (int i = 0; i <= linesNo; i++)
            points.append(getBezierPoint(data, (float)i/(float)linesNo));
        for (int j = 0; j <linesNo; j++)
            indices.append(QPoint(j, j+1));
    }

    for (int v = 0; v < V+1; v++) {
        //calculate points
        QVector4D data[ORDER];
        for (int i = 0; i<ORDER; i++)
           data[i] = getBezierPointCol(i, (v*unitY)/(unitY*V));
        //get bezier line from calculated points
        for (int i = 0; i <= linesNo; i++)
            points.append(getBezierPoint(data, (float)i/(float)linesNo));
        for (int j = 0; j <linesNo; j++)
            indices.append(QPoint(j, j+1));
    }

/*    int n = markers.length();
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
   */

}

QVector4D BicubicSegment::getBezierPointCol(int index, float t)
{
    QVector4D data[ORDER];
    for (int i = 0; i<ORDER; i++) {
        data[i] = markers[i][index]->point;
    }
    getBezierPoint(data, t);
}

QVector4D BicubicSegment::getBezierPointRow(int index, float t)
{
    QVector4D data[ORDER];
    for (int i = 0; i<ORDER; i++) {
        data[i] = markers[index][i]->point;
    }
    getBezierPoint(data, t);
}

QVector4D BicubicSegment::getBezierPoint(QVector4D points[ORDER], float t)
{
    int degree = ORDER-1;
    while (degree > 0) {
        for (int k = 0; k < degree; k++)
            points[k] = points[k] + t * ( points[k+1] - points[k] );
        degree--;
    }
    //todo w = 1;
    QVector4D p = points[0];
    return p;
}

void BicubicSegment::Clear()
{
    indices.clear();
    points.clear();
}

QVector<QPoint> BicubicSegment::getIndices() const
{
    return indices;
}

void BicubicSegment::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> BicubicSegment::getPoints() const
{
    return points;
}

void BicubicSegment::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

QList<Marker *> BicubicSegment::getMarkers() const
{
    QList<Marker*> m;
    for (int i = 0; i<ORDER; i++)
        for (int j = 0; j<ORDER; j++)
            m.append(markers[j][i]);
    return m;
}
