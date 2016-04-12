#ifndef MARKER_H
#define MARKER_H

#include <QColor>
#include <QPainter>
#include <QVector4D>
#include "cadobject.h"
#include "constants.h"

class Marker : public CADObject
{
    float size;
    static int id;
public:
    QString name;
    QString idname;
    bool IsSelected;
    QVector4D point;
    QVector4D pointWorld;
    Marker();
    //~Marker() {};
    Marker(QVector4D position);
    Marker(QVector4D position, QColor color);
    Marker(float x, float y, float z);
    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    float getSize() const;
    void setSize(float value);
    void setColor(const QColor &value);
    QColor getColor() const;
};

#endif // MARKER_H
