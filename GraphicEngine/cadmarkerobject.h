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
};

#endif // CADMARKEROBJECT_H
