#include "cadmarkerobject.h"

CADMarkerObject::CADMarkerObject()
{

}

CADMarkerObject::~CADMarkerObject()
{

}

void CADMarkerObject::InitializeBSpline(QMatrix4x4 matrix)
{
    //TO DO: IMPLEMENT VIRTUAL METHOD
    /*Clear();
    int n = markers.length();
    float length = 0;
    //initialize Curve
    for (int i = 0; i < n-1; i++) {
        pointsCurve.append(markers[i]->point);
        indicesCurve.append(QPoint(i, i+1));
        QVector4D q1 = Constants::perspectiveMatrix*matrix*markers[i]->point;
        QVector4D q2 = Constants::perspectiveMatrix*matrix*markers[i+1]->point;
        q1 = q1/q1.w();
        q2 = q2/q2.w();
        length += sqrt(pow((q2.x()-q1.x()),2)+pow((q2.y()-q1.y()),2));
    }
    pointsCurve.append(markers.last()->point);
    //divide on as many curves of the 3th deg: ex: 4 3 3 3.... 3 2/1

    if (n>0) {
        QList<Marker*> m;
        for (int i = 1; i < n; i) {
            m.append(markers[i-1]);
            for (int c = 0; c<3; c++) {
                if (i<n)
                    m.append(markers[i]);
                i++;
            }
            BezierSegments.append(Segment(m));
            m.clear();
        }
    }

    //Clear() todo: move to cadobject
    int linesNo = (int)length;
    int j=0;
    for (int s = 0; s<BezierSegments.length(); s++) {
        for (int i = 0; i <= linesNo; i++) {
            points.append(getBezierPoint(BezierSegments[s], (float)i/(float)linesNo));
        }
        for (j; j < (linesNo*(s+1)); j++) {
            indices.append(QPoint(j, j+1));
        }
    }*/

}
