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

    float Width;
    float Height;

    bool isTrimmed = false;
    QVector<QVector4D> trimData;
    bool side;
    bool isPatch1;

    CADSplinePatch();
    virtual ~CADSplinePatch()=0;
    virtual void InitializeSpline(QMatrix4x4 matrix)=0;
    virtual void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    virtual void DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    //If found in the list, replaces old reference to marker with a new one
    virtual void ReplaceMarker(Marker *toReplace, Marker *replaceWith);
    //u, v between 0-1
    virtual QVector4D ComputePos(float u, float v);
    virtual QVector4D ComputeDu(float u, float v);
    virtual QVector4D ComputeDv(float u, float v);
    virtual QVector4D ComputeDuv(float u, float v);
    virtual QVector4D ComputeDvu(float u, float v);
    virtual QVector4D ComputeDuu(float u, float v);
    virtual QVector4D ComputeDvv(float u, float v);
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
