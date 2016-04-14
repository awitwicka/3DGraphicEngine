#ifndef CURSOR_H
#define CURSOR_H

#include <QVector4D>
#include <QVector>
#include <QMatrix4x4>
#include "cadobject.h"


class Cursor : public CADObject
{
    void InitializeCursor();
    float size;
    void Clear();
public:
    float range;
    Cursor();
    //~Cursor() {};
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

    virtual QVector<QVector4D> getPoints() const;
    virtual void setPoints(const QVector<QVector4D> &value);
    virtual QVector<QPoint> getIndices() const;
    virtual void setIndices(const QVector<QPoint> &value);
};

#endif // CURSOR_H
