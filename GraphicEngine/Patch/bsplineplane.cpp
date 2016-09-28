#include "bsplineplane.h"

#define ORDER 4
#define DEGREE 3

int BSplinePlane::id = 0;

BSplinePlane::BSplinePlane()
{

}

BSplinePlane::BSplinePlane(QMatrix4x4 matrix, QList<Marker> *MainMarkers) : X(7), Y(7), H(400), R(100)
{
    Width = 500;
    Height = 400;
    U = 5;
    V = 5;
    isPlane = true;
    this->isHorizontal = true;
    name = QString("BSplinePlane%1").arg(id);
    idname = QString("k%1").arg(id);
    id++;
    InitializeMarkers(MainMarkers);
    //TODO: change name to update
    InitializeSpline(matrix);
}

BSplinePlane::BSplinePlane(QMatrix4x4 matrix, QList<Marker*> markers, float X, float Y, bool isHorizontal, bool isPlane/*, QString name*/)
{
    this->isPlane = isPlane;
    this->isHorizontal = isHorizontal;
    this->U = 4;
    this->V = 4;
    this->X = X;
    this->Y = Y;
    if (isPlane == true) {
        MarkerN = X-1+4;
        MarkerM = Y-1+4;
    } else {
        MarkerN = X-1+4-3;
        MarkerM = Y-1+4;
    }
    this->markers = markers;
    //this->name = name;
    name = QString("BezierPlane%1").arg(id);
    idname = QString("k%1").arg(id);
    id++;

     // references for drawing from deboore points
    /*if (isPlane) {
        for (int i = 0; i<markers.length(); i++)
            planeMarkers.append(markers[i]);
    } else {
        int count = 0;
        for (int i = 0; i<MarkerM; i++) {
            for (int j = 0; j<MarkerN; j++) {
                planeMarkers.append(markers[count]);
                count++;
            }
            planeMarkers.append(markers[i*MarkerN]);
            planeMarkers.append(markers[i*MarkerN+1]);
            planeMarkers.append(markers[i*MarkerN+2]);
        }
    }*/

    InitializeSpline(matrix);
}

BSplinePlane::BSplinePlane(QMatrix4x4 matrix, QList<Marker> *MainMarkers, float U, float V, float X, float Y, float Param1, float Param2, float x, float y, float z, bool isPlane)
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
        MarkerN = X-1+4;
        MarkerM = Y-1+4;
    } else {
        if (X<3 || Y<3)
            return;
        this->R = Param1;
        this->H = Param2;
        MarkerN = X-1+4-3;
        MarkerM = Y-1+4;
    }
    this->offset = QVector4D(x, y, z, 1);
    name = QString("BSplinePlane%1").arg(id);
    idname = QString("k%1").arg(id);
    id++;
    InitializeMarkers(MainMarkers);
    //TODO: change name to update
    InitializeSpline(matrix);
}

void BSplinePlane::InitializeMarkers(QList<Marker> *MainMarkers)
{
    if (isPlane) {
        float bicubicWidth = Width/X;
        float bicubicHeight = Height/Y;

        float unitX = bicubicWidth/(ORDER-1);
        float unitY = bicubicHeight/(ORDER-1);

        //create markers
        for (int y = 0; y<Y+(ORDER-1); y++) {
        for (int x = 0; x<X+(ORDER-1); x++) {
            MainMarkers->append(Marker((unitX*x)+offset.x(), (unitY*y)+offset.y(), offset.z(), false));
            markers.append(&MainMarkers->last());
        }
        }
    } else {
        const float pi = asin(1.0)*2.0;
        //float alphaSegment = 2*pi/X;
        //float alpha = alphaSegment/(ORDER-1);
        float bicubicHeight = H/Y;
        float unitY = bicubicHeight/(ORDER-1);

        //create markers
        for (int y = 0; y<Y+(ORDER-1); y++) {
        for (int x = 0; x<X+(ORDER-1)-3; x++) {
            float rad = ((2*pi/X)*(ORDER-1)) + (2*pi/X)*x;;
            //if (x==0 && y==0) {
            //    rad = (2*pi/X)*j;
            //} else if (x ==0) {
            //   rad = (2*pi/X)*j;
            //}
            MainMarkers->append(Marker(R*cos(rad)+offset.x(), R*sin(rad)+offset.y(), (unitY*y)+offset.z(), false));
            markers.append(&MainMarkers->last());
        }
        }
    }
    // references for drawing from deboore points
    if (isPlane) {
        for (int i = 0; i<markers.length(); i++)
            planeMarkers.append(markers[i]);
    } else {
        int count = 0;
        for (int i = 0; i<MarkerM; i++) {
            for (int j = 0; j<MarkerN; j++) {
                planeMarkers.append(markers[count]);
                count++;
            }
            planeMarkers.append(markers[i*MarkerN]);
            planeMarkers.append(markers[i*MarkerN+1]);
            planeMarkers.append(markers[i*MarkerN+2]);
        }
    }
}

void BSplinePlane::Clear()
{
    BezierSegments.clear();
    BezierSegMarkers.clear();
}


void BSplinePlane::InitializeSpline(QMatrix4x4 matrix)
{
    Clear();    
    //TODO: refresh beziersegmarkers only on merge points
    int count = 0;
    int line_length = 0;
    if (isPlane) {
        line_length = X+(ORDER-1);
        for (int j = 0; j<Y; j++) {
        for (int i = 0; i<X; i++) {
            count = i + (j*line_length);
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
        line_length = X+(ORDER-1)-3;
        for (int j = 0; j<Y; j++) {
        for (int i = 0; i<X; i++) {
            count = i + (j*line_length);
            BezierSegMarkers.append(QList<Marker*>());
            for (int row = 0; row<ORDER; row++) {
                for (int col = 0; col<ORDER; col++) {
                    if (col+i >= X+(ORDER-1)-3)
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

void BSplinePlane::ReplaceMarker(Marker *toReplace, Marker *replaceWith)
{
    for (int i = 0; i< markers.length(); i++) {
        if (*markers[i] == *toReplace) {
            markers.replace(i, replaceWith);
        }
    }
    planeMarkers.clear();
    if (isPlane) {
        for (int i = 0; i<markers.length(); i++)
            planeMarkers.append(markers[i]);
    } else {
        int count = 0;
        for (int i = 0; i<MarkerM; i++) {
            for (int j = 0; j<MarkerN; j++) {
                planeMarkers.append(markers[count]);
                count++;
            }
            planeMarkers.append(markers[i*MarkerN]);
            planeMarkers.append(markers[i*MarkerN+1]);
            planeMarkers.append(markers[i*MarkerN+2]);
        }
    }
}

void BSplinePlane::GetKnotVector(float &u, float &v, QVector<float> &knotVectorU, QVector<float> &knotVectorV)
{
    if(u < 0) u = 0;
    else if(u >= 1.0) u = 0.999f;
    if(v < 0) v = 0;
    else if(v >= 1.0) v = 0.999f;

    int n = X-1+4;
    int m = Y-1+4;

    int knotCountU = n + DEGREE +1;
    int knotCountV = m + DEGREE +1;

    float du = 1.0f / (float)(knotCountU - 1);
    float dv = 1.0f / (float)(knotCountV - 1);
    float uknot = 0;
    float vknot = 0;
    for(int i = 0; i < knotCountU; i++){
        knotVectorU.push_back(uknot);
        uknot += du;
    }
    for(int i = 0; i < knotCountV; i++){
        knotVectorV.push_back(vknot );
        vknot += dv;
    }
    float umin = knotVectorU[3];
    float umax = knotVectorU[knotCountU - 1 - 3];
    float ru = umax - umin;
    u = ru * u + umin;

    float vmin = knotVectorV[3];
    float vmax = knotVectorV[knotCountV - 1 - 3];
    float rv = vmax - vmin;
    v = rv * v + vmin;
}

float BSplinePlane::BsplineRecurive(float t, int n, int i, const QVector<float>& knotVector){
    if(t == knotVector[0] && i == 0) return 1.0f;
    if(t == knotVector[knotVector.size()-1] && i == knotVector.size() - n - 2) return 1.0f;

    if (n == 0){
        if(t >= knotVector[i] && t < knotVector[i+1])
            return 1;
        else
            return 0;
    }

    float leftRecursion = (t - knotVector[i]);
    float leftDenominator = (knotVector[i+n] - knotVector[i]);
    if(leftRecursion == 0 || leftDenominator == 0)
        leftRecursion = 0;
    else
        leftRecursion /= leftDenominator;

    float rightRecursion = knotVector[i+1+n] - t;
    float rightDenominator = (knotVector[i+1+n] - knotVector[i+1]);
    if(rightRecursion == 0 || rightDenominator == 0)
        rightRecursion = 0;
    else
        rightRecursion /= rightDenominator;

    leftRecursion *= BsplineRecurive(t, n-1, i, knotVector);
    rightRecursion *= BsplineRecurive(t, n-1, i+1, knotVector);

    return leftRecursion + rightRecursion;
}

float BSplinePlane::BsplineDerivativeRecurive(float t, int n, int i, const QVector<float>& knotVector){
    if(t == knotVector[0] && i == 0) return 1.0f;
    if(t == knotVector[knotVector.size()-1] && i == knotVector.size() - n - 2) return 1.0f;

    if (n == 0){
        if(t >= knotVector[i] && t < knotVector[i+1])
            return 1;
        else
            return 0;
    }

    float leftRecursion = n;
    float leftDenominator = (knotVector[i+n] - knotVector[i]);
    if(leftRecursion == 0 || leftDenominator == 0)
        leftRecursion = 0;
    else
        leftRecursion /= leftDenominator;

    float rightRecursion = n;
    float rightDenominator = (knotVector[i+1+n] - knotVector[i+1]);
    if(rightRecursion == 0 || rightDenominator == 0)
        rightRecursion = 0;
    else
        rightRecursion /= rightDenominator;

    leftRecursion *= BsplineRecurive(t, n-1, i, knotVector);
    rightRecursion *= BsplineRecurive(t, n-1, i+1, knotVector);

    return leftRecursion - rightRecursion;
}

QVector4D BSplinePlane::ComputePos(float u, float v)
{
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineRecurive(u, DEGREE, i, knotVectorU);
            float bsV = BsplineRecurive(v, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsU * bsV * pos;
        }
    }
    return result;
}

QVector4D BSplinePlane::ComputeDu(float u, float v){
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineDerivativeRecurive(u, DEGREE, i, knotVectorU);
            float bsV = BsplineRecurive(v, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsU * bsV * pos;
        }
    }
    return result;
}

QVector4D BSplinePlane::ComputeDuu(float u, float v){
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineDerivativeRecurive(u, DEGREE, i, knotVectorU);
            float bsUU = BsplineDerivativeRecurive(bsU, DEGREE, i, knotVectorU);
            float bsV = BsplineRecurive(v, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsUU * bsV * pos;
        }
    }
    return result;
}

QVector4D BSplinePlane::ComputeDuv(float u, float v){
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineDerivativeRecurive(u, DEGREE, i, knotVectorU);
            float bsV = BsplineDerivativeRecurive(v, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsU * bsV * pos;
        }
    }
    return result;
}

QVector4D BSplinePlane::ComputeDv(float u, float v){
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineRecurive(u, DEGREE, i, knotVectorU);
            float bsV = BsplineDerivativeRecurive(v, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsU * bsV * pos;
        }
    }
    return result;
}

QVector4D BSplinePlane::ComputeDvv(float u, float v){
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineRecurive(u, DEGREE, i, knotVectorU);
            float bsV = BsplineDerivativeRecurive(v, DEGREE, j, knotVectorV);
            float bsVV = BsplineDerivativeRecurive(bsV, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsU * bsVV * pos;
        }
    }
    return result;
}

QVector4D BSplinePlane::ComputeDvu(float u, float v){
    QVector<float> knotVectorU;
    QVector<float> knotVectorV;
    GetKnotVector(u,v,knotVectorU,knotVectorV);

    int n = X-1+4;
    int m = Y-1+4;

    QVector4D result = QVector4D(0,0,0,0);
    for(int i = 0; i < n; i++){ //ROWCOUNT X/u
        for(int j = 0; j < m; j++){ //COLCOUNT Y/v
            float bsU = BsplineDerivativeRecurive(u, DEGREE, i, knotVectorU);
            float bsV = BsplineDerivativeRecurive(v, DEGREE, j, knotVectorV);
            QVector4D pos = planeMarkers[i+j*n]->point;
            result += bsU * bsV * pos;
        }
    }
    return result;
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

QList<Marker*> BSplinePlane::getMarkers()
{
    /*QList<Marker*> m;
    for (int i = 0; i<markers.length(); i++)
        m.append(&markers[i]);
    return m;*/
    return markers;
}
