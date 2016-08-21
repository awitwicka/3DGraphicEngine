#include "gregorypatch.h"

GregoryPatch::GregoryPatch()
{

}

GregoryPatch::GregoryPatch(QList<Marker> *m, int u, int v, QMatrix4x4 matrix)
{
    //TODO: create markers and construct default gregory paych
}

GregoryPatch::GregoryPatch(QList<Marker*> gapMarkers, int u, int v, QMatrix4x4 matrix)
{
    this->markers = gapMarkers;
}

void GregoryPatch::InitializeSpline(QMatrix4x4 matrix)
{

}
