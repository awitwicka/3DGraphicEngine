#include "cadsplinepatch.h"

CADSplinePatch::CADSplinePatch()
{

}

void CADSplinePatch::DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{

}

void CADSplinePatch::DrawVectors(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{

}

void CADSplinePatch::ReplaceMarker(Marker *toReplace, Marker *replaceWith)
{
    //TODO: try to make a common method
}

QVector4D CADSplinePatch::ComputePos(float u, float v)
{
    return QVector4D(-1,-1,-1,-1);
}

void CADSplinePatch::Clear() {

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

