#include "bsplineplane.h"

#define ORDER 4

int BSplinePlane::id = 0;

BSplinePlane::BSplinePlane()
{

}

BSplinePlane::BSplinePlane(QMatrix4x4 matrix) : Width(500), Height(400), X(7), Y(7), H(400), R(100)
{
    U = 5;
    V = 5;
    isPlane = true;
    name = QString("BezierPlane %1").arg(id);
    idname = QString("g%1").arg(id);
    id++;
    InitializeMarkers();
    //TODO: change name to update
    InitializeSpline(matrix);
}

BSplinePlane::BSplinePlane(QMatrix4x4 matrix, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane)
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

void BSplinePlane::InitializeMarkers()
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
                    if (j<ORDER-1 && y==0 && x!=0) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*(X) + (x-1)].at(i*ORDER + j+1));
                    //weź z góry
                    } else if (i<ORDER-1 && y>0) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[(y-1)*(X) + x].at((i+1)*ORDER + j));
                    } else if (i==ORDER-1 && x>0 && y>0 && j<ORDER-1) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*(X) + (x-1)].at(i*ORDER + j+1));
                    } else {
                        if (x==0 && y==0)
                            markers.append(Marker((unitX*j)+(bicubicWidth*x)+offset.x(), (unitY*i)+(bicubicHeight*y)+offset.y(), offset.z(), false));
                        else if (y==0)
                            markers.append(Marker((unitX*x)+(bicubicWidth)+offset.x(), (unitY*i)+(bicubicHeight*0)+offset.y(), offset.z(), false));
                        else if (x==0)
                            markers.append(Marker((unitX*j)+(bicubicWidth*0)+offset.x(), (unitY*y)+(bicubicHeight)+offset.y(), offset.z(), false));
                        else
                            markers.append(Marker((unitX*x)+(bicubicWidth)+offset.x(), (unitY*y)+(bicubicHeight)+offset.y(), offset.z(), false));
                        BezierSegMarkers[y*(X) + x].append(&markers[count]);
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
                    if (j<ORDER-1 && y==0 && x!=0) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*(X) + (x-1)].at(i*ORDER + j+1));
                    //weź z góry
                    } else if (i<ORDER-1 && y>0) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[(y-1)*(X) + x].at((i+1)*ORDER + j));
                    } else if (i==ORDER-1 && x>0 && y>0 && j<ORDER-1) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*(X) + (x-1)].at(i*ORDER + j+1));
                    //connect front with back
                    } else if (j==(ORDER-1) && x==(X-3)) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*X].at(i*ORDER));
                    } else if (j==(ORDER-1) && x==(X-2)) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*X + 1].at(i*ORDER));
                    } else if (j==(ORDER-1) && x==(X-1)) {
                        BezierSegMarkers[y*(X) + x].append(BezierSegMarkers[y*X + 2].at(i*ORDER));
                    } else {
                        float rad = ((2*pi/X)*(ORDER-1)) + (2*pi/X)*x;
                        if (x==0 && y==0) {
                            rad = (2*pi/X)*j;
                        } else if (x ==0) {
                            rad = (2*pi/X)*j;
                        }
                        if (y==0)
                            markers.append(Marker(R*cos(rad)+offset.x(), R*sin(rad)+offset.y(), (unitY*i)+(bicubicHeight*y)+offset.z(), false));
                        else
                            markers.append(Marker(R*cos(rad)+offset.x(), R*sin(rad)+offset.y(), (unitY*y)+(bicubicHeight)+offset.z(), false));
                        BezierSegMarkers[y*(X) + x].append(&markers[count]);
                        count++;
                }
            }
        }
        }
    }
}

void BSplinePlane::Clear()
{
    BezierSegments.clear();
}


void BSplinePlane::InitializeSpline(QMatrix4x4 matrix)
{
    Clear();
    for (int i = 0; i<BezierSegMarkers.length(); i++)
         BezierSegments.append(BicubicSegment(&BezierSegMarkers[i], U, V, matrix, false));
}

void BSplinePlane::Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    for (int i = 0; i<BezierSegments.length(); i++)
        BezierSegments[i].Draw(painter, matrix, isStereo);
}

void BSplinePlane::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    for (int i = 0; i<BezierSegments.length(); i++)
        BezierSegments[i].DrawPolygon(painter, matrix, isStereo);
}

QVector<QPoint> BSplinePlane::getIndices() const
{
    return indices;
}

void BSplinePlane::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> BSplinePlane::getPoints() const
{
    return points;
}

void BSplinePlane::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

QList<Marker> BSplinePlane::getMarkers()
{
    /*QList<Marker*> m;
    for (int i = 0; i<markers.length(); i++)
        m.append(&markers[i]);
    return m;*/
    return markers;
}
