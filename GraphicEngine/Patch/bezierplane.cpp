#include "bezierplane.h"

#define ORDER 4

int BezierPlane::id = 0;

BezierPlane::BezierPlane()
{

}

BezierPlane::BezierPlane(QMatrix4x4 matrix, QList<Marker> *MainMarkers) : X(7), Y(7), H(400), R(100)
{
    Width = 500;
    Height = 400;
    U = 5;
    V = 5;
    isPlane = true;
    isHorizontal = true;
    name = QString("BezierPlane%1").arg(id);
    idname = QString("g%1").arg(id);
    id++;
    InitializeMarkers(MainMarkers);
    //TODO: change name to update
    InitializeSpline(matrix);
}

BezierPlane::BezierPlane(QMatrix4x4 matrix, QList<Marker*> markers, float X, float Y, bool isHorizontal, bool isPlane/*, QString name*/)
{
    this->isPlane = isPlane;
    this->isHorizontal = isHorizontal;
    this->U = 4;
    this->V = 4;
    this->X = X;
    this->Y = Y;
    if (isPlane == true) {
        MarkerN = (X-1)*3+4;
        MarkerM = (Y-1)*3+4;
    } else {
        MarkerN = (X-1)*3+4-1;
        MarkerM = (Y-1)*3+4;
    }
    this->markers = markers;
    //this->name = name;
    name = QString("BezierPlane%1").arg(id);
    idname = QString("g%1").arg(id);
    id++;
    InitializeSpline(matrix);
}

BezierPlane::BezierPlane(QMatrix4x4 matrix, QList<Marker> *MainMarkers, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane)
{
    this->isPlane = isPlane;
    this->isHorizontal = true;
    this->U = U;
    this->V = V;
    this->X = X;
    this->Y = Y;
    if (isPlane == true) {
        this->Width = Param1;
        this->Height = Param2;
        MarkerN = (X-1)*3+4;
        MarkerM = (Y-1)*3+4;
    } else {
        this->R = Param1;
        this->H = Param2;
        MarkerN = (X-1)*3+4-1;
        MarkerM = (Y-1)*3+4;
    }
    this->offset = QVector4D(x, y, z, 1);
    name = QString("BezierPlane%1").arg(id);
    idname = QString("g%1").arg(id);
    id++;
    InitializeMarkers(MainMarkers);
    //TODO: change name to update
    InitializeSpline(matrix);
}

void BezierPlane::InitializeMarkers(QList<Marker> *MainMarkers)
{
    if (isPlane) {
        float bicubicWidth = Width/X;
        float bicubicHeight = Height/Y;

        float unitX = bicubicWidth/(ORDER-1);
        float unitY = bicubicHeight/(ORDER-1);

        //create markers
        for (int y = 0; y<Y*(ORDER-1)+1; y++) {
        for (int x = 0; x<X*(ORDER-1)+1; x++) {
            MainMarkers->append(Marker((unitX*x)+offset.x(), (unitY*y)+offset.y(), offset.z(), false));
            markers.append(&MainMarkers->last());
        }
        }
    } else {
        const float pi = asin(1.0)*2.0;
        float alphaSegment = 2*pi/X;
        float alpha = alphaSegment/(ORDER-1);
        float bicubicHeight = H/Y;
        float unitY = bicubicHeight/(ORDER-1);

        //create markers
        for (int y = 0; y<Y*(ORDER-1)+1; y++) {
        for (int x = 0; x<X*(ORDER-1); x++) {
            float rad = alpha*x;
            MainMarkers->append(Marker(R*cos(rad)+offset.x(), R*sin(rad)+offset.y(), (unitY*y)+offset.z()-(H/2.0f), false));
            markers.append(&MainMarkers->last());
        }
        }
    }
}

void BezierPlane::Clear()
{
    if(!BezierSegments.isEmpty())
        BezierSegments.clear();
    if(!BezierSegMarkers.isEmpty())
        BezierSegMarkers.clear();
}


void BezierPlane::InitializeSpline(QMatrix4x4 matrix)
{   
    Clear();    
    //TODO: refresh beziersegmarkers only on merge points
    int count = 0;
    int line_length = 0;
    if (isPlane) {
        line_length = X*(ORDER-1)+1;
        for (int j = 0; j<Y; j++) {
        for (int i = 0; i<X; i++) {
            count = (i*3) + (j*line_length*3);
            BezierSegMarkers.append(QList<Marker*>());
            for (int row = 0; row<ORDER; row++) {
                for (int col = 0; col<ORDER; col++) {
                    BezierSegMarkers.last().append(markers[count+col]);
                }
                count+=line_length;//next line
            }
        }
        }
    } else {
        line_length = X*(ORDER-1);
        for (int j = 0; j<Y; j++) {
        for (int i = 0; i<X; i++) {
            count = (i*3) + (j*line_length*3);
            BezierSegMarkers.append(QList<Marker*>());
            for (int row = 0; row<ORDER; row++) {
                for (int col = 0; col<ORDER; col++) {
                    if (i==X-1 && col==ORDER-1)
                        BezierSegMarkers.last().append(markers[count+col-line_length]); //warp
                    else
                        BezierSegMarkers.last().append(markers[count+col]);
                }
                count+=line_length;//next line
            }
        }
        }
    }

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

void BezierPlane::ReplaceMarker(Marker *toReplace, Marker *replaceWith)
{
    //Clear();
    QString idname = toReplace->idname;
    for (int i = 0; i< markers.length(); i++) {
        if (*markers[i] == *toReplace) { //CHECK: compare references instead
             //markers[i] = replaceWith;//&w->markers[FindIndexByRef(&m)];
            markers.replace(i, replaceWith);
        }
         //   return 0;
    }
    //InitializeSpline();
    //return -1; if nothing
}


QList<int> BezierPlane::ConvertUVtoLocal(float &u, float &v)
{
    if(u < 0) u = 0;
    else if(u >= 1.0) u = 0.999f;
    if(v < 0) v = 0;
    else if(v >= 1.0) v = 0.999f;

    //get indexes of the bezier patch that contains given u,v    0|---b0---|---b1---|-- .... --|---bn---|1
    int j = u * X;
    int i = v * Y;

    //get u v parameters of the beggining and end of the patch
    float minJ = (float)j / (float)X; //poczadek i kniec param platka w 0-1
    float maxJ = (float)(j+1) / (float)X;
    float minI = (float)i / (float)Y;
    float maxI = (float)(i+1) / (float)Y;

    //get height/width of the patch (should actually always be equal to 1/X 1/Y)
    float rJ = maxJ - minJ;
    float rI = maxI - minI;

    //get u/v coordinates on the patch (conversion to 0-1 on the patch)
    u = (u - minJ) / rJ;
    v = (v - minI) / rI;

    //   u
    // v  _j0___j1___j2_
    //   |    |    |    |
    // i0|____|____|____|
    //   |    |    |    |
    // i1|    |    |    |
    //
    QList<int> xy;
    xy.append(j);
    xy.append(i);
    return xy;
}


QVector4D BezierPlane::ComputePos(float u, float v)
{
   QList<int> indexXY = ConvertUVtoLocal(u, v);
   return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierPoint(u, v);
}

QVector4D BezierPlane::ComputeDu(float u, float v)
{
    QList<int> indexXY = ConvertUVtoLocal(u, v);
    return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierDuPoint(u, v);
}

QVector4D BezierPlane::ComputeDv(float u, float v)
{
    QList<int> indexXY = ConvertUVtoLocal(u, v);
    return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierDvPoint(u, v);
}

QVector4D BezierPlane::ComputeDuv(float u, float v)
{
    QList<int> indexXY = ConvertUVtoLocal(u, v);
    return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierDuvPoint(u, v);
}

QVector4D BezierPlane::ComputeDvu(float u, float v)
{
    QList<int> indexXY = ConvertUVtoLocal(u, v);
    return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierDvuPoint(u, v);
}

QVector4D BezierPlane::ComputeDuu(float u, float v)
{
    QList<int> indexXY = ConvertUVtoLocal(u, v);
    return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierDuuPoint(u, v);
}

QVector4D BezierPlane::ComputeDvv(float u, float v)
{
    QList<int> indexXY = ConvertUVtoLocal(u, v);
    return BezierSegments[indexXY[0]+(indexXY[1]*X)].getBezierDvvPoint(u, v);
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

QList<Marker*> BezierPlane::getMarkers()
{
    /*QList<Marker*> m;
    for (int i = 0; i<markers.length(); i++)
        m.append(&markers[i]);
    return m;*/
    return markers;
}
