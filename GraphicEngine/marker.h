#ifndef MARKER_H
#define MARKER_H

#include <QColor>
#include <QPoint>



class Marker
{
    float size;
    QString name;
    QPoint point;
    QColor color;
public:
    Marker(float x, float y, float z);
};

#endif // MARKER_H
