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

    //find first 2 lines of markers (facing gap to the top)
    QVector4D line1[ORDER];
    QVector4D line2[ORDER];

    // c ------0------ a
    GetFirst2Lines(c, a, patches[0], line1, line2); //TODO: check if not pass through reference
    QList<QVector4D> cast1 = DeCasteljau(line1);
    QList<QVector4D> cast2 = DeCasteljau(line2); //check if not have to individually for each point
    int count = 0;
    for (int i = 0; i<cast1.length(); i++) {
        pointsVectors.append(cast1[i]);
        QVector4D dir = cast1[i] - cast2[i];
        pointsVectors.append(cast1[i] +  dir); //move point along dir
        indicesVectors.append(QPoint(count, count+1));
        count +=2;
    }
    // a ------1------ b
    GetFirst2Lines(a, b, patches[1], line1, line2);
    cast1.clear();
    cast2.clear();
    cast1 = DeCasteljau(line1);
    cast2 = DeCasteljau(line2); //check if not have to individually for each point
    for (int i = 0; i<cast1.length(); i++) {
        pointsVectors.append(cast1[i]);
        QVector4D dir = cast1[i] - cast2[i];
        pointsVectors.append(cast1[i] +  dir); //move point along dir
        indicesVectors.append(QPoint(count, count+1));
        count +=2;
    }
    // b ------2------ c
    cast1.clear();
    cast2.clear();
    GetFirst2Lines(b, c, patches[2], line1, line2);
    cast1 = DeCasteljau(line1);
    cast2 = DeCasteljau(line2); //check if not have to individually for each point
    for (int i = 0; i<cast1.length(); i++) {
        pointsVectors.append(cast1[i]);
        QVector4D dir = cast1[i] - cast2[i];
        pointsVectors.append(cast1[i] +  dir); //move point along dir
        indicesVectors.append(QPoint(count, count+1));
        count +=2;
    }
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
