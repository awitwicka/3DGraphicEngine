#include "marker.h"


QColor Marker::getColor() const
{
    return color;
}

Marker::Marker()
{
    size = 4;
    point = QVector4D(0, 0, 0, 1);
    name = QString("point %1").arg(id);
    id++;
}

Marker::Marker(float x, float y, float z) : color(Qt::white),  highlighColor(Qt::yellow), size(10)
{
    name = QString("point %1").arg(id);// + "1");
    //point(x, y,  z);
    point = QVector4D(x, y, z, 1);
    id++;
}

float Marker::getSize() const
{
    return size;
}

void Marker::setSize(float value)
{
    size = value;
}

void Marker::setColor(const QColor &value)
{
    color = value;
}

int Marker::id = 0;
