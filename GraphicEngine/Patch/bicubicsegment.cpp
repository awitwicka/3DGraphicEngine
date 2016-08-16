#include "bicubicsegment.h"

BicubicSegment::BicubicSegment()
{

}

BicubicSegment::BicubicSegment(QList<Marker *>* m, int u, int v, QMatrix4x4 matrix, bool isBezier)
{
    if (m->length() != ORDER*ORDER)
        return;
    this->isBezier = isBezier;
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

void BicubicSegment::InitializeBezierMarkers()
{
    for (int n  = 0; n<ORDER; n++) {
    QList<VirtualMarkInfo> level1;
    for (int i = 0; i < (ORDER)-1; i++) {
        QVector4D q1 = markers[i][n]->point;
        QVector4D q2 = markers[i+1][n]->point;

        VirtualMarkInfo v13;
        v13.position = q1 + (1.0f/3.0f)*(q2 - q1);
        v13.parent = markers[i][n];
        v13.partner = markers[i+1][n];
        level1.append(v13);

        VirtualMarkInfo v23;
        v23.position = q1 + (2.0f/3.0f)*(q2 - q1);
        v23.parent = markers[i+1][n];
        v23.partner = markers[i][n];
        level1.append(v23);
    }
    //level2
    QList<VirtualMarkInfo> level2;
    for (int i = 0; i<ORDER-2; i++) {
        QVector4D q1 = level1[i*2+1].position;
        QVector4D q2 = level1[(i+1)*2].position;

        VirtualMarkInfo v12;
        v12.position = q1 + 0.5f*(q2 - q1);
        v12.parent = markers[i+1][n];
        v12.partner = nullptr;
        level2.append(v12);
    }
    //bezier
    int i = 0;
    bezierMarkers[0][n]=Marker(level2[i].position);
    bezierMarkers[1][n]=Marker(level1[(i+1)*2].position);
    bezierMarkers[2][n]=Marker(level1[(i+1)*2+1].position);
    bezierMarkers[3][n]=Marker(level2[i+1].position);
    }
    /***************************************************/
    for (int n  = 0; n<ORDER; n++) {
    QList<VirtualMarkInfo> level1;
    for (int i = 0; i < (ORDER)-1; i++) {
        QVector4D q1 = bezierMarkers[n][i].point;
        QVector4D q2 = bezierMarkers[n][i+1].point;

        VirtualMarkInfo v13;
        v13.position = q1 + (1.0f/3.0f)*(q2 - q1);
        level1.append(v13);

        VirtualMarkInfo v23;
        v23.position = q1 + (2.0f/3.0f)*(q2 - q1);
        level1.append(v23);
    }
    //level2
    QList<VirtualMarkInfo> level2;
    for (int i = 0; i<ORDER-2; i++) {
        QVector4D q1 = level1[i*2+1].position;
        QVector4D q2 = level1[(i+1)*2].position;

        VirtualMarkInfo v12;
        v12.position = q1 + 0.5f*(q2 - q1);
        v12.partner = nullptr;
        level2.append(v12);
    }
    //bezier
    int i = 0;
    bezierMarkers[n][0]=Marker(level2[i].position);
    bezierMarkers[n][1]=Marker(level1[(i+1)*2].position);
    bezierMarkers[n][2]=Marker(level1[(i+1)*2+1].position);
    bezierMarkers[n][3]=Marker(level2[i+1].position);
    }
}

void BicubicSegment::InitializeSpline(QMatrix4x4 matrix)
{
    //float unitX = segLengthX/U;
    //float unitY = segLengthY/V;
    Clear();
    if (isBezier) {
        for (int y = 0; y<ORDER; y++) {
            for (int x = 0; x<ORDER; x++) {
                bezierMarkers[x][y] = markers[x][y]->point;
            }
        }
    } else {
            InitializeBezierMarkers();
    }
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

    //BSPLINE
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
        data[i] = bezierMarkers[index][i].point;
    }
    return getBezierPoint(data, t);
}

QVector4D BicubicSegment::getBezierPointRow(int index, float t)
{
    QVector4D data[ORDER];
    for (int i = 0; i<ORDER; i++) {
        data[i] = bezierMarkers[i][index].point; //x y
    }
    return getBezierPoint(data, t);
}

QVector4D BicubicSegment::getBezierPoint(QVector4D pkt[ORDER], float t)
{
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
    indicesCurve.clear();
    pointsCurve.clear();
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
