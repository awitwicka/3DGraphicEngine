#include "bezier.h"

Bezier::Bezier()
{

}

Bezier::Bezier(const QList<Marker *> &markers)
{
    //divide on as many curves of the 3th deg: ex: 4 3 3 3.... 3 2/1
    //TODO: why deg = 1 why alwa 2 points, why 13178416941 in markers???????
    int n = markers.length();
    if (n>0) {
        QList<Marker*> m;
        for (int i = 1; i < n; i) {
            m.append(markers[i-1]);
            for (int c = 0; c<3; c++) {
                if (i<n)
                    m.append(markers[i]);
                i++;
            }
            Segments.append(Segment(m));
            m.clear();
        }
    }
    InitializeBezier();
}

void Bezier::InitializeBezier()
{
    //Clear(); todo: move to cadobject
    int linesNo = 100;
    int j=0;
    for (int s = 0; s<Segments.length(); s++) {
        for (int i = 0; i <= linesNo; i++) {
            points.append(getBezierPoint(Segments[s], (float)i/(float)linesNo));
        }
        for (j; j < (linesNo*(s+1))-1; j++) {
            indices.append(QPoint(j, j+1));
        }
    }
}

QVector4D Bezier::getBezierPoint(Segment seg, float t)
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

QVector<QPoint> Bezier::getIndices() const
{
    return indices;
}

void Bezier::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> Bezier::getPoints() const
{
    return points;
}

void Bezier::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

/*
 * vec2 getBezierPoint( vec2* points, int numPoints, float t ) {
    vec2* tmp = new vec2[numPoints];
    memcpy(tmp, points, numPoints * sizeof(vec2));
    int i = numPoints - 1;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        i--;
    }
    vec2 answer = tmp[0];
    delete[] tmp;
    return answer;
}
*/

