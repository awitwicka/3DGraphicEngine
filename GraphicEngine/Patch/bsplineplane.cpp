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
    name = QString("BSplinePlane%1").arg(id);
    idname = QString("k%1").arg(id);
    id++;

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
            MainMarkers->append(Marker(R*cos(rad)+offset.x(), R*sin(rad)+offset.y(), (unitY*y)+offset.z()-(H/4.0f), false));
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
    indices.clear();
    points.clear();
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
    
    for (int i = 0; i<BezierSegMarkers.length(); i++) {
        if (!isTrimmed)
            BezierSegments.append(BicubicSegment(&BezierSegMarkers[i], U, V, matrix, false));
        else {
            //BezierSegments.append(BicubicSegment(&BezierSegMarkers[i], U, V, matrix, false, trimData, side, isPatch1));

            count = 0;
            int linesNo=100;// = (int)length;
            for (int u = 0; u < U+1; u++) {
                for (int i = 0; i <= linesNo; i++) {
                    if (IsEnclosed(u/(float)U, i/(float)linesNo, trimData, isPatch1)) {
                        points.append(ComputePos(u/(float)U, i/(float)linesNo));
                        indices.append(QPoint(count, count+1));
                        count++;
                    }
                }
                if (indices.count()>0)
                    indices.removeLast();
            }
            linesNo = 100;
            for (int v = 0; v < V+1; v++) {
                for (int i = 0; i <= linesNo; i++) {
                    if (IsEnclosed(i/(float)linesNo, v/(float)V, trimData, isPatch1)) {
                        points.append(ComputePos(i/(float)linesNo, v/(float)V));
                        indices.append(QPoint(count, count+1));
                        count++;
                    }
                }
                if (indices.count()>0)
                    indices.removeLast();
            }

        }
    }
}

void BSplinePlane::Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    if (!isTrimmed) {
        for (int i = 0; i<BezierSegments.length(); i++)
            BezierSegments[i].Draw(painter, matrix, isStereo);
    } else {
        QVector<QPoint> indices = getIndices();
        QVector<QVector4D> points = getPoints();
        for (int i = 0; i < indices.length(); i++) {
            QVector4D q1 = points[indices[i].x()]; //TODO: refactoring -> to function
            QVector4D q2 = points[indices[i].y()];
            q1 = matrix*q1;
            q2 = matrix*q2;
            if (q1.z() <= -Constants::Rpersp && q2.z() <= -Constants::Rpersp) {
               continue;
            }
            if (q1.z() <= -Constants::Rpersp && q2.z() > -Constants::Rpersp) {
                QVector4D dir = q1-q2;
                QVector4D newq = dir * (-Constants::Rpersp+1-q2.z())/dir.z();
                q1 = q2 + newq; //watch not to change w parameter while adding newq
            } else if (q1.z() > -Constants::Rpersp && q2.z() <= -Constants::Rpersp) {
                QVector4D dir = q2-q1;
                QVector4D newq = dir * (-Constants::Rpersp+1-q1.z())/dir.z();
                q2 = q1 + newq;
            }
            if (q1.z() > -Constants::Rpersp && q2.z() > -Constants::Rpersp) {
                if (isStereo) {
                    QVector4D L1  = Constants::stereoLMatrix*q1;
                    QVector4D L2  = Constants::stereoLMatrix*q2;
                    QVector4D R1  = Constants::stereoRMatrix*q1;
                    QVector4D R2  = Constants::stereoRMatrix*q2;
                    L1 = L1/L1.w();
                    L2 = L2/L2.w();
                    R1 = R1/R1.w();
                    R2 = R2/R2.w();
                    painter.setCompositionMode(QPainter::CompositionMode_Plus);
                    painter.setPen(LRedColor);
                    painter.drawLine(L1.x(),L1.y(),L2.x(),L2.y());
                    painter.setPen(RBlueColor);
                    painter.drawLine(R1.x(),R1.y(),R2.x(),R2.y());
                }
                else {
                    painter.setPen(Color);
                    q1 = Constants::perspectiveMatrix*q1;
                    q2 = Constants::perspectiveMatrix*q2;
                    q1 = q1/q1.w();
                    q2 = q2/q2.w();
                    painter.drawLine(q1.x(),q1.y(),q2.x(),q2.y());
                }
            }
        }
    }
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

double BSplinePlane::GetAngle(QPointF pivot, QPointF source, QPointF dest)
{
    QPointF s = QPointF(source.x() - pivot.x(), source.y() - pivot.y());
    QPointF d = QPointF(dest.x() - pivot.x(), dest.y() - pivot.y());

    float dotProduct = s.x() * d.x() + s.y() * d.y();
    float crossProduct = s.x() * d.y() - s.y() * d.x();

    return atan2(crossProduct, dotProduct);
}

bool BSplinePlane::IsEnclosed(float u, float v, QVector<QVector4D> polygon, bool isPatch1)
{
    double det = 0;
    double totalAngle = 0;
    float epsilon = 0.01f;
    // If subject it's on polygon edge, it's inside of polygon.
    for(int i = 0; i < polygon.count(); i++)
    {
        QVector4D from = polygon[i];
        QVector4D to = polygon[(i + 1) % polygon.count()];

        if(isPatch1) {
            /*det = from.x() * to.y() + to.x() * v + u * from.y() - u * to.y() - from.x() * v - to.x() * from.y();
            if(det == 0)
            {
                if ((fmin(from.x(), to.x()) <= u) && (u <= fmax(from.x(), to.x())) &&
                    (fmin(from.y(), to.y()) <= v) && (v <= fmax(from.y(), to.y())))
                    return (side)? true : false;
            }*/
            totalAngle = GetAngle(QPointF(u,v), QPointF(polygon[polygon.count() - 1].x(), polygon[polygon.count() - 1].y()), QPointF(polygon[0].x(), polygon[0].y()));
            for (int i = 0; i < polygon.count() - 1; i++)
                totalAngle += GetAngle(QPointF(u,v), QPointF(polygon[i].x(), polygon[i].y()), QPointF(polygon[i + 1].x(), polygon[i + 1].y()));
        }
        else {
            /*det = from.z() * to.w() + to.z() * v + u * from.w() - u * to.w() - from.z() * v - to.z() * from.w();
            if(det == 0)
            {
                if ((fmin(from.z(), to.z()) <= u) && (u <= fmax(from.z(), to.z())) &&
                    (fmin(from.w(), to.w()) <= v) && (v <= fmax(from.w(), to.w())))
                    return (side)? true : false;
            }*/
            totalAngle = GetAngle(QPointF(u,v), QPointF(polygon[polygon.count() - 1].z(), polygon[polygon.count() - 1].w()), QPointF(polygon[0].z(), polygon[0].w()));
            for (int i = 0; i < polygon.count() - 1; i++)
                totalAngle += GetAngle(QPointF(u,v), QPointF(polygon[i].z(), polygon[i].w()), QPointF(polygon[i + 1].z(), polygon[i + 1].w()));
        }

    }


    bool condition;
    side? condition = (fabs(totalAngle) < epsilon) : condition = (fabs(totalAngle) >= epsilon);

    if (condition)
        return false;
    else
        return true;
    //return !totalAngle.EpsilonEquals(0);
}
