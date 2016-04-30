#ifndef CADSPLINEPATCH_H
#define CADSPLINEPATCH_H

#include "marker.h"

class CADSplinePatch
{
public:
    QString name;
    QString idname;
    QList<Marker> markers;

    CADSplinePatch();
    virtual ~CADSplinePatch()=0;
    virtual void InitializeSpline(QMatrix4x4 matrix)=0;
    virtual void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
};

#endif // CADSPLINEPATCH_H
