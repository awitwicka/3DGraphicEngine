#ifndef MARKER_H
#define MARKER_H

#include <QColor>
#include <QVector4D>



class Marker
{
    float size;
    QString name;
    QColor color;
public:
    QVector4D point;
    Marker();
    Marker(float x, float y, float z);

    float getSize() const;
    void setSize(float value);
};

#endif // MARKER_H
