#include "bezierplane.h"

#define ORDER 3+1

int BezierPlane::id = 0;

BezierPlane::BezierPlane()
{

}

BezierPlane::BezierPlane(QMatrix4x4 matrix) : U(5), V(5), Width(500), Height(400), X(3), Y(3)
{
    name = QString("BezierPlane %1").arg(id);
    idname = QString("bp%1").arg(id);
    id++;
    InitializeMarkers();
    //TODO: change name to update
    InitializeSpline(matrix);
}

void BezierPlane::InitializeMarkers()
{
    BezierSegMarkers.clear();
    float bicubicWidth = Width/X;
    float bicubicHeight = Height/Y;

    float unitX = bicubicWidth/ORDER-1;
    float unitY = bicubicHeight/ORDER-1;

    int count = 0;
    for (int y = 0; y<Y; y++) {
    for (int x = 0; x<X; x++) {
        BezierSegMarkers.append(QList<Marker*>());
        for (int i = 0; i<ORDER; i++)
            for (int j = 0; j<ORDER; j++) {                
                //weź z komórki po lewej
                /*if (j==0 && x>0) {
                     BezierSegMarkers[y*(Y) + x].append(&markers[i*(ORDER) + j]);
                //weź z góry
                } else if (i==0 && y>0) {
                     BezierSegMarkers[y*(Y) + x].append(&markers[i*(ORDER) + j]);
                } else {*/
                    markers.append(Marker((unitX*j)+(bicubicWidth*x), (unitY*i)+(bicubicHeight*y), 0));
                    BezierSegMarkers[y*(Y) + x].append(&markers[/*i*(ORDER) + j*/count]);
                    count++;
                //BezierSegMarkers[y*(Y) + x].append(&markers[/*i*(ORDER) + j*/count]);
            }
        }
    }
    }
//}

void BezierPlane::Clear()
{
    BezierSegments.clear();
}


void BezierPlane::InitializeSpline(QMatrix4x4 matrix)
{
    Clear();
    for (int i = 0; i<BezierSegMarkers.length(); i++)
         BezierSegments.append(BicubicSegment(&BezierSegMarkers[i], U, V, matrix));
}

void BezierPlane::Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    for (int i = 0; i<BezierSegments.length(); i++)
        BezierSegments[i].Draw(painter, matrix, isStereo);
}

QVector<QPoint> BezierPlane::getIndices() const
{
    return indices;
}

void BezierPlane::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> BezierPlane::getPoints() const
{
    return points;
}

void BezierPlane::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

QList<Marker> BezierPlane::getMarkers()
{
    /*QList<Marker*> m;
    for (int i = 0; i<markers.length(); i++)
        m.append(&markers[i]);
    return m;*/
    return markers;
}
