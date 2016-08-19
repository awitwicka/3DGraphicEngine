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
    int degree = ORDER-1;
    QVector4D tmp[ORDER];
    for (int i = 0; i<ORDER; i++)
        tmp[i] = pkt[i];


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

QList<QVector4D> GapFilling::DeCasteljauTree(QVector4D pkt[ORDER], float t)
{
    QList<QVector4D> result;
    int degree = ORDER-1;
    result.append(pkt[0]);
    result.append(pkt[ORDER-1]);

    while (degree > 0) {
        int k =0;
        for (k = 0; k < degree; k++) {
            pkt[k] = pkt[k] + t * ( pkt[k+1] - pkt[k] );
        }
        int listMid = result.length()/2;
        if (degree>1) {
            result.insert(listMid,pkt[0]);
            result.insert(listMid+1,pkt[k]);
        }
        degree--;
    }
    int listMid = result.length()/2;
    result.insert(listMid,pkt[0]);
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
                line2[i] = patch->BezierSegments[0].bezierMarkers[i][1].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointCol(i, 1.0f/6.0f);//w ktora strone biegnie u i v?
            //todo: did i confused col with row?
            }
        else if (indexOfC == 12)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[0][i].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[1][i].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointRow(i, 1.0f/6.0f);
            }
        break;
    case 3:
        if (indexOfC == 0)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][0].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[i][1].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointCol(i, 1.0f/6.0f);
            }
        else if (indexOfC == 15)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[3][i].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[2][i].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointRow(i, (1.0f/6.0f)*5.0f);
            }
        break;
    case 12:
        if (indexOfC == 0)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[0][i].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[1][i].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointRow(i, 1.0f/6.0f);
            }
        else if (indexOfC == 15)
            for (int i = 0; i<ORDER; i++) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][3].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[i][2].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointCol(i, (1.0f/6.0f)*5.0f);
            }
         break;
    case 15:
        if (indexOfC == 3)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[3][i].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[2][i].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointRow(i, (1.0f/6.0f)*5.0f);
            }
        else if (indexOfC == 12)
            for (int i = ORDER-1; i>=0; i--) {
                line1[i] = patch->BezierSegments[0].bezierMarkers[i][3].point;
                line2[i] = patch->BezierSegments[0].bezierMarkers[i][2].point;
                //line2[i] = patch->BezierSegments[0].getBezierPointCol(i, (1.0f/6.0f)*5.0f);
            }
        break;
    default:
        //TODO: Throw exception
        break;
    }
}

QVector4D GapFilling::ComputeBorderControlPoints(Marker *a, Marker *c, CADSplinePatch *patch, QList<QVector4D> &tree1, QList<QVector4D> &tree3)
{
    //find first 2 lines of markers (facing gap to the top)
    QVector4D line1[ORDER];
    QVector4D line2[ORDER];
    GetFirst2Lines(a, c, patch, line1, line2); //TODO: check if not pass through reference

    //cast1 = DeCasteljau(line1);
    //control points at border of a patch
    /*QList<QVector4D>*/ tree1 = DeCasteljauTree(line1, 0.5f);
    //control points inside of a patch
    QList<QVector4D> tree2 = DeCasteljauTree(line2, 0.5f);
    //control points inside of a gap
    //QList<QVector4D> cast3;

    GetFirst2Lines(a, c, patch, line1, line2);
    QList<QVector4D> decast1 = DeCasteljau(line1);
    QList<QVector4D> decast2 = DeCasteljau(line2);

    int count = indicesVectors.length()*2;
    for (int i = 0; i<tree1.length(); i++) {
        QVector4D dir = (tree1[i] - tree2[i])/2.0f;
        tree3.append(tree1[i] +  dir);

        QVector4D vectDir = decast1[i] - decast2[i];
        pointsVectors.append(decast1[i]);
        pointsVectors.append(decast1[i] + vectDir/2.0f/*vectDir.normalized()*10*/); //normalize??
        indicesVectors.append(QPoint(count, count+1));
        count +=2;
    }
    QVector4D p1 = tree1[3];
    QVector4D d1 = tree3[3];
    QVector4D p2 = p1 + (3.0f/2.0f)*(d1 - p1);
    //tmp
    pointsVectors.append(p1);
    pointsVectors.append(p2);
    indicesVectors.append(QPoint(count, count+1));
    //
    return p2;
}

QList<QVector4D> GapFilling::ComputeMiddleControlPoints(QVector4D b0, QVector4D a3, QVector4D b3, QVector4D Bezier[ORDER])
{
    QVector4D c0 = Bezier[1] - Bezier[0];
    QVector4D c1 = Bezier[2] - Bezier[1];

    QVector4D b1 = (c0 + 4*b0 + b3 + a3)/6.0f;
    QVector4D b2 = (c1 + b0 + b3 + a3)/3.0f;

    QList<QVector4D> result;
    result.append(b1);
    result.append(b2);

    return result;
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
    //control points at border of a patch
    QList<QVector4D> cp0a;
    //control points inside of a gap
    QList<QVector4D> cp1a;
    QVector4D p2a = ComputeBorderControlPoints(c, a, patches[0], cp0a, cp1a);
    // a ------1------ b
    QList<QVector4D> cp0b;
    QList<QVector4D> cp1b;
    QVector4D p2b = ComputeBorderControlPoints(a, b, patches[1], cp0b, cp1b);
    // b ------2------ c
    QList<QVector4D> cp0c;
    QList<QVector4D> cp1c;
    QVector4D p2c = ComputeBorderControlPoints(b, c, patches[2], cp0c, cp1c);

    // Middle points preview:
    // |cp0[3](p1)---cp1[3](d1)---p2---d2---midP
    // |P0-----------P1-----------x----P2---P3

    QVector4D midP = (p2a + p2b + p2c)/3.0f;
    QVector4D d2a = midP + (2.0f/3.0f)*(p2a - midP);
    QVector4D d2b = midP + (2.0f/3.0f)*(p2b - midP);
    QVector4D d2c = midP + (2.0f/3.0f)*(p2c - midP);

    QVector4D B1[ORDER];
    B1[0] = cp0a[3];
    B1[1] = cp1a[3];
    B1[2] = d2a;
    B1[3] = midP;
    QVector4D B2[ORDER];
    B2[0] = cp0b[3];
    B2[1] = cp1b[3];
    B2[2] = d2b;
    B2[3] = midP;
    QVector4D B3[ORDER];
    B3[0] = cp0c[3];
    B3[1] = cp1c[3];
    B3[2] = d2c;
    B3[3] = midP;

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

    QList<QVector4D> middleA = ComputeMiddleControlPoints(cp0a[4]-cp0a[3], midP-B3[2], B2[2]-midP, B1);
    QList<QVector4D> middleB = ComputeMiddleControlPoints(cp0b[4]-cp0b[3], midP-B1[2], B3[2]-midP, B2);
    QList<QVector4D> middleC = ComputeMiddleControlPoints(cp0c[4]-cp0c[3], midP-B2[2], B1[2]-midP, B3);
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
