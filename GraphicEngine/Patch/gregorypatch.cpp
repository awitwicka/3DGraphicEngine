#include "gregorypatch.h"

int GregoryPatch::id = 0;

GregoryPatch::GregoryPatch()
{

}

GregoryPatch::GregoryPatch(QList<Marker> *m, int u, int v, QMatrix4x4 matrix)
{
    //TODO: create markers and construct default gregory paych
    name = QString("GregoryPatch%1").arg(id);
    idname = QString("y%1").arg(id);
    U = u;
    V = v;
    InitializeSpline(matrix);
}

GregoryPatch::GregoryPatch(QList<Marker*> gapMarkers, int u, int v, QMatrix4x4 matrix)
{
    U = u;
    V = v;
    this->markers = gapMarkers;
    // 0___1___2___3
    // |   |   |   |
    // |   6   7   |
    // 4--5     8--9
    // |           |
    // 10-11   14-15
    // |   12 13   |
    // |___|___|___|
    // 16  17  18  19
    InitializeSpline(matrix);
}

void GregoryPatch::InitializeSpline(QMatrix4x4 matrix)
{
    Clear();
    //calc lines number in curves and curvepath
    //int n = ORDER;
    //float length = 0;
    //float lengthX [ORDER];
    //float lengthY [ORDER];

    //POLYGON
    //obwód
    int count = 0;
    for (int i = 0; i<ORDER; i++) {
        pointsCurve.append(markers[i]->point);
        indicesCurve.append(QPoint(count, count+1));
        count++;
    }
    pointsCurve.append(markers[9]->point);
    indicesCurve.append(QPoint(count, count+1));
    count++;
    pointsCurve.append(markers[15]->point);
    indicesCurve.append(QPoint(count, count+1));
    count++;
    for (int i = 19; i>=16; i--) {
        pointsCurve.append(markers[i]->point);
        indicesCurve.append(QPoint(count, count+1));
        count++;
    }
    pointsCurve.append(markers[10]->point);
    indicesCurve.append(QPoint(count, count+1));
    count++;
    pointsCurve.append(markers[4]->point);
    indicesCurve.append(QPoint(count, count+1));
    pointsCurve.append(markers[0]->point);
    count +=2;
    //middle points
    pointsCurve.append(markers[1]->point);
    pointsCurve.append(markers[6]->point);
    indicesCurve.append(QPoint(count, count+1));
    count+=2;
    pointsCurve.append(markers[2]->point);
    pointsCurve.append(markers[7]->point);
    indicesCurve.append(QPoint(count, count+1));
    count+=2;
    pointsCurve.append(markers[12]->point);
    pointsCurve.append(markers[17]->point);
    indicesCurve.append(QPoint(count, count+1));
    count+=2;
    pointsCurve.append(markers[13]->point);
    pointsCurve.append(markers[18]->point);
    indicesCurve.append(QPoint(count, count+1));
    /******/
    count+=2;
    pointsCurve.append(markers[4]->point);
    pointsCurve.append(markers[5]->point);
    indicesCurve.append(QPoint(count, count+1));
    count+=2;
    pointsCurve.append(markers[10]->point);
    pointsCurve.append(markers[11]->point);
    indicesCurve.append(QPoint(count, count+1));
    count+=2;
    pointsCurve.append(markers[8]->point);
    pointsCurve.append(markers[9]->point);
    indicesCurve.append(QPoint(count, count+1));
    count+=2;
    pointsCurve.append(markers[14]->point);
    pointsCurve.append(markers[15]->point);
    indicesCurve.append(QPoint(count, count+1));



    /*for (int x = 0; x<ORDER; x++) {
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
    }*/

    //GREGORY
    count = 0;
    int linesNo=100;// = (int)length;
    for (int u = 0; u < U+1; u++) {
        //calculate points
        QVector4D data[ORDER];

        //get bezier line from calculated points
        for (int i = 0; i <= linesNo; i++) {
            for (int i = 0; i<ORDER; i++)
               data[i] = getBezierPointRow(i, u/(float)U, (float)i/(float)linesNo);
            points.append(getBezierPoint(data, (float)i/(float)linesNo));
        }
        for (int j = 0; j <linesNo; j++) {
            indices.append(QPoint(count, count+1));
            count++;
        }
        count++;
    }
    for (int v = 0; v < V+1; v++) {
        //calculate points
        QVector4D data[ORDER];

        //get bezier line from calculated points
        for (int i = 0; i <= linesNo; i++) {
            for (int i = 0; i<ORDER; i++)
               data[i] = getBezierPointCol(i, (float)i/(float)linesNo, (v/**unitY*/)/(float)(/*unitY**/V));
            points.append(getBezierPoint(data, (float)i/(float)linesNo));
        }
        for (int j = 0; j <linesNo; j++) {
            indices.append(QPoint(count, count+1));
            count++;
        }
        count++;
    }
}

void GregoryPatch::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::green;
    Draw(painter, matrix, isStereo, pointsCurve, indicesCurve);
    Color = Qt::white;
}

QVector4D GregoryPatch::getBezierPointCol(int index, float u, float v) //v od dolu czy od gory idzie??
{   // 0___1___2___3
    // |   |   |   |
    // |   6   7   |
    // 4--5     8--9
    // |           |
    // 10-11   14-15
    // |   12 13   |
    // |___|___|___|
    // 16  17  18  19
    QVector4D v00 = (u*markers[12]->point + v*markers[11]->point)/(u+v); // 11/12
    QVector4D v01 = ((1-u)*markers[13]->point + v*markers[14]->point)/((1-u)+v); // 13/14
    QVector4D v10 = (u*markers[6]->point + (1-v)*markers[5]->point)/(u+(1-v)); // 6/5
    QVector4D v11 = ((1-u)*markers[7]->point + (1-v)*markers[8]->point)/((1-u)+(1-v)); // 7/8

    QVector4D data[ORDER];
    if (index == 0) {
        data[0] = markers[0]->point;
        data[1] = markers[4]->point;
        data[2] = markers[10]->point;
        data[3] = markers[16]->point;
    } else if (index == 1) {
        data[0] = markers[1]->point;
        data[1] = v10;
        data[2] = v00;
        data[3] = markers[17]->point;
    } else if (index == 2) {
        data[0] = markers[2]->point;
        data[1] = v11;
        data[2] = v01;
        data[3] = markers[18]->point;
    } else if (index == 3) {
        data[0] = markers[3]->point;
        data[1] = markers[9]->point;
        data[2] = markers[15]->point;
        data[3] = markers[19]->point;
    }
    return getBezierPoint(data, v);
}

QVector4D GregoryPatch::getBezierPointRow(int index, float u, float v)
{
   // 0___1___2___3
   // |   |   |   |
   // |   6   7   |
   // 4--5     8--9
   // |           |
   // 10-11   14-15
   // |   12 13   |
   // |___|___|___|
   // 16  17  18  19

   QVector4D v00 = (u*markers[12]->point + v*markers[11]->point)/(u+v); // 11/12
   QVector4D v01 = ((1-u)*markers[13]->point + v*markers[14]->point)/((1-u)+v); // 13/14
   QVector4D v10 = (u*markers[6]->point + (1-v)*markers[5]->point)/(u+(1-v)); // 6/5
   QVector4D v11 = ((1-u)*markers[7]->point + (1-v)*markers[8]->point)/((1-u)+(1-v)); // 7/8

   QVector4D data[ORDER];
   if (index == 0) {
       data[0] = markers[0]->point;
       data[1] = markers[1]->point;
       data[2] = markers[2]->point;
       data[3] = markers[3]->point;
   } else if (index == 1) {
       data[0] = markers[4]->point;
       data[1] = v10;
       data[2] = v11;
       data[3] = markers[9]->point;
   } else if (index == 2) {
       data[0] = markers[10]->point;
       data[1] = v00;
       data[2] = v01;
       data[3] = markers[15]->point;
   } else if (index == 3) {
       data[0] = markers[16]->point;
       data[1] = markers[17]->point;
       data[2] = markers[18]->point;
       data[3] = markers[19]->point;
   }
   return getBezierPoint(data, u);
}

QVector4D GregoryPatch::getBezierPoint(QVector4D pkt[ORDER], float t)
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

void GregoryPatch::Clear()
{
    indices.clear();
    points.clear();
    indicesCurve.clear();
    pointsCurve.clear();
}
