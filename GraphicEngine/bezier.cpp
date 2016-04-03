#include "bezier.h"

Bezier::Bezier()
{

}

Bezier::Bezier(Marker*m1, Marker*m2, Marker*m3, Marker*m4)
{
    markers.append(m1);
    markers.append(m2);
    markers.append(m3);
    markers.append(m4);
    InitializeBezier();
}

void Bezier::InitializeBezier()
{
    //Clear(); todo: move to cadobject
    int SegmentsNo = 100;
    for (int i = 1; i <= SegmentsNo; i++) {
        points.append(getBezierPoint(*markers[0],*markers[1],*markers[2],*markers[3],i/SegmentsNo));
    }
    for (int i = 0; i < SegmentsNo-1; i++) {
        indices.append(QPoint(i, i+1));
    }
}

QVector4D Bezier::getBezierPoint(Marker m1, Marker m2, Marker m3, Marker m4, float t)
{
    int degree = 3;
    QVector4D tmp[degree+1];
    tmp[0] = m1.point;
    tmp[1] = m2.point;
    tmp[2] = m3.point;
    tmp[3] = m4.point;

    while (degree > 0) {
        for (int k = 0; k < degree; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        degree--;
    }
    //todo w = 1;
    return tmp[0];
}

/*
 * vec2 getBezierPoint( vec2* points, int numPoints, float t ) {
    vec2* tmp = new vec2[numPoints];
    memcpy(tmp, points, numPoints * sizeof(vec2));
    int i = numPoints - 1;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        i--;
    }
    vec2 answer = tmp[0];
    delete[] tmp;
    return answer;
}
*/
