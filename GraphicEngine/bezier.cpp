#include "bezier.h"

int Bezier::id = 0;


//TODO clear pointers
/*
 *
 * int iNum = qList.count();
for (int i=0; i<iNumElements; i++)
  delete (qList.takeAt(0));
 *
 * or qdelete all
 *
 * */

Bezier::Bezier()
{
    name = QString("bezier %1").arg(id);
    idname = QString("b%1").arg(id);
    id++;
}

Bezier::Bezier(const QList<Marker *> &mark, QMatrix4x4 matrix)
{
    name = QString("bezier %1").arg(id);
    idname = QString("b%1").arg(id);
    id++;
    markers = mark;
    InitializeBezier(matrix);
}

void Bezier::DrawCurve(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::green;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

void Bezier::InitializeBezier(QMatrix4x4 matrix)
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

void Bezier::ChangeToBSpline()
{

}

void Bezier::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
    BezierSegments.clear(); //todo:are they disposed?
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

