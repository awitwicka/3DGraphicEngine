#include "marker.h"

Marker::Marker()
{
    size = 4;
    point = QVector4D(0, 0, 0, 1);
}

Marker::Marker(float x, float y, float z) : color(Qt::white), size(10)
{
   // name = point + "1";
    //point(x, y,  z);
    point = QVector4D(x, y, z, 1);
}

float Marker::getSize() const
{
    return size;
}

void Marker::setSize(float value)
{
    size = value;
}
