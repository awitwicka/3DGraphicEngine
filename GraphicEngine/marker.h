#ifndef MARKER_H
#define MARKER_H

#include <QColor>
#include <QVector4D>



class Marker
{
    float size;
    QColor color;
    static int id;
public:
    QString name;
    QVector4D point;
    Marker();
    Marker(float x, float y, float z);

    float getSize() const;
    void setSize(float value);
};

#endif // MARKER_H
