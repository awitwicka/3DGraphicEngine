#ifndef CURSOR_H
#define CURSOR_H

#include <QVector4D>
#include <QVector>
#include <QMatrix4x4>
#include "cadobject.h"


class Cursor : public CADObject
{
    float size;
    void Clear();
public:
    float range;
    void InitializeCursor();
    Cursor();
    //~Cursor() {};
    QVector<QVector4D> points;
    QVector<QPoint> indices;
    void updateCursor(QMatrix4x4 world);
    float getSize() const;
    void setSize(float value);
    QVector4D center;
    //draw cursor
    /*float offset = cursor.getSize()/2;
    painter.setPen(Qt::red);
    painter.drawLine(cursor.center.x()-offset, cursor.center.y(), cursor.center.x()+offset, cursor.center.y());
    painter.setPen(Qt::green);
    painter.drawLine(cursor.center.x(), cursor.center.y()-offset, cursor.center.x(), cursor.center.y()+offset);
    painter.setPen(Qt::blue);
    painter.drawLine(cursor.center.x()-offset, cursor.center.y(), cursor.center.x()+offset, cursor.center.y());*/
};

#endif // CURSOR_H
