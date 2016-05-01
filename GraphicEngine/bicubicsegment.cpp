#include "bicubicsegment.h"

BicubicSegment::BicubicSegment()
{

}

BicubicSegment::BicubicSegment(QList<Marker *>* m, int u, int v, QMatrix4x4 matrix)
{
    if (m->length() != ORDER*ORDER)
        return;
    U = u;
    V = v;
    //segLengthX = width;
    //segLengthY = height;
    for (int y = 0; y<ORDER; y++) {
        for (int x = 0; x<ORDER; x++) {
            markers[x][y] = (*m)[y*ORDER + x];
        }
    }
    InitializeSpline(matrix);
}

void BicubicSegment::InitializeSpline(QMatrix4x4 matrix)
{
    //float unitX = segLengthX/U;
    //float unitY = segLengthY/V;
    Clear();
    //calc lines number in curves and curvepath
    int n = ORDER;
    float length = 0;
    float lengthX [ORDER];
    float lengthY [ORDER];
    //initialize Curve
    int count = 0;
    for (int x = 0; x<ORDER; x++) {
        for (int i = 0; i < n-1; i++) {
            pointsCurve.append(markers[x][i]->point);
            indicesCurve.append(QPoint(count, count+1));
            QVector4D q1 = Constants::perspectiveMatrix*matrix*markers[x][i]->point;
            QVector4D q2 = Constants::perspectiveMatrix*matrix*markers[x][i+1]->point;
            q1 = q1/q1.w();
            q2 = q2/q2.w();
            lengthX[x] += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
            count++;
        }
        count++;
        pointsCurve.append(markers[x][n-1]->point);
    }
    for (int y = 0; y<ORDER; y++) {
        for (int i = 0; i < n-1; i++) {
            pointsCurve.append(markers[i][y]->point);
            indicesCurve.append(QPoint(count, count+1));
            QVector4D q1 = Constants::perspectiveMatrix*matrix*markers[i][y]->point;
            QVector4D q2 = Constants::perspectiveMatrix*matrix*markers[i+1][y]->point;
            q1 = q1/q1.w();
            q2 = q2/q2.w();
            lengthY[y] += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
            count++;
        }
        count++;
        pointsCurve.append(markers[n-1][y]->point);
    }

    count = 0;
    int linesNo=100;// = (int)length;
    for (int u = 0; u < U+1; u++) {
        //calculate points
        QVector4D data[ORDER];
        for (int i = 0; i<ORDER; i++)
           data[i] = getBezierPointRow(i, u/(float)U);
        //get bezier line from calculated points
        for (int i = 0; i <= linesNo; i++)
            points.append(getBezierPoint(data, (float)i/(float)linesNo));
        for (int j = 0; j <linesNo; j++) {
            indices.append(QPoint(count, count+1));
            count++;
        }
        count++;
    }
    for (int v = 0; v < V+1; v++) {
        //calculate points
        QVector4D data[ORDER];
        for (int i = 0; i<ORDER; i++)
           data[i] = getBezierPointCol(i, (v/**unitY*/)/(float)(/*unitY**/V));
        //get bezier line from calculated points
        for (int i = 0; i <= linesNo; i++)
            points.append(getBezierPoint(data, (float)i/(float)linesNo));
        for (int j = 0; j <linesNo; j++) {
            indices.append(QPoint(count, count+1));
            count++;
        }
        count++;
    }
}

void BicubicSegment::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::green;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

QVector4D BicubicSegment::getBezierPointCol(int index, float t)
{
    QVector4D data[ORDER];
    for (int i = 0; i<ORDER; i++) {
        data[i] = markers[i][index]->point;
    }
    return getBezierPoint(data, t);
}

QVector4D BicubicSegment::getBezierPointRow(int index, float t)
{
    QVector4D data[ORDER];
    for (int i = 0; i<ORDER; i++) {
        data[i] = markers[index][i]->point;
    }
    return getBezierPoint(data, t);
}

QVector4D BicubicSegment::getBezierPoint(QVector4D pkt[ORDER], float t)
{
    /*QVector4D* tmp = new QVector4D[seg.markers.length()];
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
    return p;*/

    int degree = ORDER-1;
    while (degree > 0) {
        for (int k = 0; k < degree; k++)
            pkt[k] = pkt[k] + t * ( pkt[k+1] - pkt[k] );
        degree--;
    }
    //todo w = 1;
    QVector4D p = pkt[0];
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
