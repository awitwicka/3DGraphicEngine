#include "bezierplane.h"

#define ORDER 4

int BezierPlane::id = 0;

BezierPlane::BezierPlane()
{

}

BezierPlane::BezierPlane(QMatrix4x4 matrix) : U(5), V(5), Width(500), Height(400), X(7), Y(7), H(400), R(100)
{
    isPlane = true;
    name = QString("BezierPlane %1").arg(id);
    idname = QString("g%1").arg(id);
    id++;
    InitializeMarkers();
    //TODO: change name to update
    InitializeSpline(matrix);
}

BezierPlane::BezierPlane(QMatrix4x4 matrix, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane)
{
    this->isPlane = isPlane;
    this->U = U;
    this->V = V;
    this->X = X;
    this->Y = Y;
    if (isPlane == true) {
        this->Width = Param1;
        this->Height = Param2;
    } else {
        this->R = Param1;
        this->H = Param2;
    }
    this->offset = QVector4D(x, y, z, 1);
    name = QString("BezierPlane %1").arg(id);
    idname = QString("g%1").arg(id);
    id++;
    InitializeMarkers();
    //TODO: change name to update
    InitializeSpline(matrix);
}

void BezierPlane::InitializeMarkers()
{
    BezierSegMarkers.clear();
    if (isPlane) {
        float bicubicWidth = Width/X;
        float bicubicHeight = Height/Y;

        float unitX = bicubicWidth/(ORDER-1);
        float unitY = bicubicHeight/(ORDER-1);
        int count = 0;
        for (int y = 0; y<Y; y++) {
        for (int x = 0; x<X; x++) {
            BezierSegMarkers.append(QList<Marker*>());
            for (int i = 0; i<ORDER; i++)
                for (int j = 0; j<ORDER; j++) {
                    //weź z komórki po lewej
                    if (j==0 && x>0) {
                        int l = i*ORDER + ORDER-1;
                        BezierSegMarkers[y*(Y) + x].append(BezierSegMarkers[y*(Y) + (x-1)].at(i*ORDER + ORDER-1));
                    //weź z góry
                    } else if (i==0 && y>0) {
                        BezierSegMarkers[y*(Y) + x].append(BezierSegMarkers[(y-1)*(Y) + x].at((ORDER-1)*ORDER + j));
                    } else {
                        markers.append(Marker((unitX*j)+(bicubicWidth*x)+offset.x(), (unitY*i)+(bicubicHeight*y)+offset.y(), offset.z()));
                        BezierSegMarkers[y*(Y) + x].append(&markers[count]);
                        count++;
                }
            }
        }
        }
    } else {
        const float pi = asin(1.0)*2.0;
        float alphaSegment = 2*pi/X;
        float alpha = alphaSegment/(ORDER-1);
        float bicubicHeight = H/Y;
        float unitY = bicubicHeight/(ORDER-1);

        int count = 0;
        for (int y = 0; y<Y; y++) {
        for (int x = 0; x<X; x++) {
            BezierSegMarkers.append(QList<Marker*>());
            for (int i = 0; i<ORDER; i++)
                for (int j = 0; j<ORDER; j++) {
                    //weź z komórki po lewej
                    if (j==0 && x>0) {
                        BezierSegMarkers[y*(Y) + x].append(BezierSegMarkers[y*(Y) + (x-1)].at(i*ORDER + ORDER-1));
                    //weź z góry
                    } else if (i==0 && y>0) {
                        BezierSegMarkers[y*(Y) + x].append(BezierSegMarkers[(y-1)*(Y) + x].at((ORDER-1)*ORDER + j));
                    } else if (j==(ORDER-1) && x==(X-1)) {
                        BezierSegMarkers[y*(Y) + x].append(BezierSegMarkers[y*Y].at(i*ORDER));
                    } else {
                        float rad = alpha*j+alphaSegment*x;
                        markers.append(Marker(R*cos(rad)+offset.x(), R*sin(rad)+offset.y(), (unitY*i)+(bicubicHeight*y)+offset.z()));
                        BezierSegMarkers[y*(Y) + x].append(&markers[count]);
                        count++;
                }
            }
        }
        }
    }
}

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

void BezierPlane::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    for (int i = 0; i<BezierSegments.length(); i++)
        BezierSegments[i].DrawPolygon(painter, matrix, isStereo);
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
