#ifndef GREGORYPATCH_H
#define GREGORYPATCH_H

#include "cadsplinepatch.h"



class GregoryPatch : public CADObject, public CADSplinePatch
{
    /*Marker* TR[2]; //top right points
    Marker* TL[2]; //top left points
    Marker* BR[2]; //bottom right points
    Marker* BL[2]; //bottom left points*/
public:
    GregoryPatch();
    GregoryPatch(QList<Marker>* m, int u, int v, QMatrix4x4 matrix);
    GregoryPatch(QList<Marker*> gapMarkers, int u, int v, QMatrix4x4 matrix);

    void InitializeMarkers();
    void InitializeSpline(QMatrix4x4 matrix);


};

#endif // GREGORYPATCH_H
