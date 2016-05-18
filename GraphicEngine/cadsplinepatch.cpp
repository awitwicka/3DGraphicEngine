#include "cadsplinepatch.h"

CADSplinePatch::CADSplinePatch()
{

}

void CADSplinePatch::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{

}

int CADSplinePatch::getV() const
{
    return V;
}

void CADSplinePatch::setV(int value)
{
    V = value;
}

int CADSplinePatch::getU() const
{
    return U;
}

void CADSplinePatch::setU(int value)
{
    U = value;
}

void CADSplinePatch::InitializeSpline(QMatrix4x4 matrix)
{
    
}

CADSplinePatch::~CADSplinePatch()
{
    
}

