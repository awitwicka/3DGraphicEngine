#include "gapfilling.h"

int GapFilling::id = 0;

GapFilling::GapFilling()
{

}

GapFilling::GapFilling(QMatrix4x4 matrix, QList<CADSplinePatch *> patches)
{
    name = QString("GapFilling%1").arg(id);
    idname = QString("z%1").arg(id);
    id++;
    this->patches = patches;
    InitializeSpline(matrix);
}

GapFilling::GapFilling(QMatrix4x4 matrix, CADSplinePatch *patch1, CADSplinePatch *patch2, CADSplinePatch *patch3)
{
    name = QString("GapFilling%1").arg(id);
    idname = QString("z%1").arg(id);
    id++;
    patches.append(patch1);
    patches.append(patch2);
    patches.append(patch3);
    InitializeSpline(matrix);

}

Marker *GapFilling::FindFirstCommonMarker(CADSplinePatch *patch1, CADSplinePatch *patch2)
{
    for (int i = 0; i<patch1->markers.length(); i++) {
        for (int j = 0; j<patch2->markers.length(); j++) {
            if (patch1->markers[i] == patch2->markers[j])
                return patch1->markers[i];
        }
    }
    return nullptr;
}

QList<QVector4D> GapFilling::DeCasteljau(QVector4D pkt[ORDER])
{
    QList<QVector4D> result;
    QVector4D tmp[ORDER];
    for (int i = 0; i<ORDER; i++)
        tmp[i] = pkt[i];
    int degree = ORDER-1;
    /*while (degree > 0) {
        for (int k = 0; k < degree; k++) {
            pkt[k] = pkt[k] + t * ( pkt[k+1] - pkt[k] );
            int listMid = result.length()/2;
            if (degree>1) {
                result.insert(listMid,pkt[0]);
                result.insert(listMid+1,pkt[k]);
            }
        }
        degree--;
    }
    int listMid = result.length()/2;
    result.insert(listMid,pkt[0]);*/
    float t=0;
    for (int i = 0; i<7; i++) {
        t=(1.0f/6.0f)*i;
        while (degree > 0) {
            for (int k = 0; k < degree; k++)
                pkt[k] = pkt[k] + t * ( pkt[k+1] - pkt[k] );
            degree--;
        }
        result.append(pkt[0]);
        degree = ORDER-1;
        for (int i = 0; i<ORDER; i++)
            pkt[i] = tmp[i];
    }
    return result;
}

void GapFilling::GetFirst2Lines(Marker* a, Marker* c, CADSplinePatch* patch, QVector4D *line1, QVector4D *line2)
{
    //TODO: check if getBezierPointRow is not switched with getBezierPointCol
    int indexOfA = patch->markers.indexOf(a);
    int indexOfC = patch->markers.indexOf(c);
    switch(indexOfA) {
    case 0:
        if (indexOfC == 3)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][0].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[i][1].point;
                line2[i] = patch->BezierSegments[0].getBezierPointCol(i, 1.0f/6.0f);//w ktora strone biegnie u i v?
            //todo: did i confused col with row?
            }
        else if (indexOfC == 12)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[0][i].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[1][i].point;
                line2[i] = patch->BezierSegments[0].getBezierPointRow(i, 1.0f/6.0f);
            }
        break;
    case 3:
        if (indexOfC == 0)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][0].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[i][1].point;
                line2[i] = patch->BezierSegments[0].getBezierPointCol(i, 1.0f/6.0f);
            }
        else if (indexOfC == 15)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[3][i].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[2][i].point;
                line2[i] = patch->BezierSegments[0].getBezierPointRow(i, (1.0f/6.0f)*5.0f);
            }
        break;
    case 12:
        if (indexOfC == 0)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[0][i].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[1][i].point;
                line2[i] = patch->BezierSegments[0].getBezierPointRow(i, 1.0f/6.0f);
            }
        else if (indexOfC == 15)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][3].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[i][2].point;
                line2[i] = patch->BezierSegments[0].getBezierPointCol(i, (1.0f/6.0f)*5.0f);
            }
         break;
    case 15:
        if (indexOfC == 3)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[3][i].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[2][i].point;
                line2[i] = patch->BezierSegments[0].getBezierPointRow(i, (1.0f/6.0f)*5.0f);
            }
        else if (indexOfC == 12)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][3].point;
                //line2[i] = patch->BezierSegments[0].bezierMarkers[i][2].point;
                line2[i] = patch->BezierSegments[0].getBezierPointCol(i, (1.0f/6.0f)*5.0f);
            }
        break;
    default:
        //TODO: Throw exception
        break;
    }
}

QVector4D GapFilling::ComputeBorderControlPoints(Marker *a, Marker *c, CADSplinePatch *patch)
{
    //find first 2 lines of markers (facing gap to the top)
    QVector4D line1[ORDER];
    QVector4D line2[ORDER];
    GetFirst2Lines(a, c, patch, line1, line2); //TODO: check if not pass through reference

    //control points at border of a patch
    QList<QVector4D> cast1 = DeCasteljau(line1);
    //control points inside of a patch
    QList<QVector4D> cast2 = DeCasteljau(line2);
    //control points inside of a gap
    QList<QVector4D> cast3;

    int count = indicesVectors.length()*2;
    for (int i = 0; i<cast1.length(); i++) {
        pointsVectors.append(cast1[i]);
        QVector4D dir = cast1[i] - cast2[i];
        cast3.append(cast1[i] +  dir);
        pointsVectors.append(cast3[i]); //normalize??
        indicesVectors.append(QPoint(count, count+1));
        count +=2;
    }
    QVector4D p1 = cast1[3];
    QVector4D d1 = cast3[3];
    QVector4D p2 = p1 + (3.0f/2.0f)*(d1 - p1);
    //tmp
    pointsVectors.append(p1);
    pointsVectors.append(p2);
    indicesVectors.append(QPoint(count, count+1));
    //
    return p2;
}

void GapFilling::InitializeSpline(QMatrix4x4 matrix)
{
    //TODO: if not becubic quit
    Clear();
    //find common markers in 3 patches
    Marker* a = FindFirstCommonMarker(patches[0], patches[1]);
    Marker* b = FindFirstCommonMarker(patches[1], patches[2]);
    Marker* c = FindFirstCommonMarker(patches[2], patches[0]);

    if (a == nullptr || b == nullptr || c == nullptr)
        return;
    // c ------0------ a
    QVector4D p2a = ComputeBorderControlPoints(c, a, patches[0]);
    // a ------1------ b
    QVector4D p2b = ComputeBorderControlPoints(a, b, patches[1]);
    // b ------2------ c
    QVector4D p2c = ComputeBorderControlPoints(b, c, patches[2]);

    QVector4D midP = (p2a + p2b + p2c)/3.0f;

    //tmp
    int count = indicesVectors.length()*2;
    pointsVectors.append(p2a);
    pointsVectors.append(midP);
    indicesVectors.append(QPoint(count, count+1));
    count+=2;
    pointsVectors.append(p2b);
    pointsVectors.append(midP);
    indicesVectors.append(QPoint(count, count+1));
    count+=2;
    pointsVectors.append(p2c);
    pointsVectors.append(midP);
    indicesVectors.append(QPoint(count, count+1));
    count+=2;
    //

    //new f get g  returns
    /****************************/
    //CALCULATE CONTROL POINTS IN THE MIDDLE OF A GAP


    //QVector4D d2 = q + (2.0f/3.0f)*(p2 - q);
}

void GapFilling::DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    Color = Qt::red;
    Draw(painter, matrix, isStereo, pointsVectors, indicesVectors);
    Color = Qt::white;
}

void GapFilling::Clear()
{
    //indices.clear();
    //points.clear();
    //indicesCurve.clear();
    //pointsCurve.clear();
    indicesVectors.clear();
    pointsVectors.clear();
}
