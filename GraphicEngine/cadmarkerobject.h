#ifndef CADMARKEROBJECT_H
#define CADMARKEROBJECT_H

#include "marker.h"
#include <QString>



class CADMarkerObject
{
public:
    QString name;
    QString idname;
    QList<Marker*> markers;

    CADMarkerObject();
    virtual ~CADMarkerObject()=0;
    virtual void InitializeSpline(QMatrix4x4 matrix)=0;
    virtual void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
};

#endif // CADMARKEROBJECT_H
