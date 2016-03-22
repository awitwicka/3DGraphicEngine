#ifndef MARKER_H
#define MARKER_H

#include <QColor>
#include <QVector4D>



class Marker
{
    float size;
    QColor color;
    QColor highlighColor;
    static int id;
public:
    QString name;
    QVector4D point;
    QVector4D pointWorld;
    Marker();
    Marker(float x, float y, float z);

    float getSize() const;
    void setSize(float value);
    void setColor(const QColor &value);
    QColor getColor() const;
};

#endif // MARKER_H
