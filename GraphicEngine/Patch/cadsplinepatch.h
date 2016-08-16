#ifndef CADSPLINEPATCH_H
#define CADSPLINEPATCH_H

#include "bicubicsegment.h"
#include "marker.h"

class CADSplinePatch
{

public:
    QString name;
    QString idname;
    QList<Marker*> markers;
    QList<BicubicSegment> BezierSegments;

    CADSplinePatch();
    virtual ~CADSplinePatch()=0;
    virtual void InitializeSpline(QMatrix4x4 matrix)=0;
    virtual void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    virtual void DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    //If found in the list, replaces old reference to marker with a new one
    virtual void ReplaceMarker(Marker *toReplace, Marker *replaceWith);
    virtual void Clear();

    virtual int getU() const;
    virtual void setU(int value);
    virtual int getV() const;
    virtual void setV(int value);

    bool isPlane;
    bool isHorizontal;
    int MarkerN;
    int MarkerM;
protected:
    int U;
    int V;
};

#endif // CADSPLINEPATCH_H
