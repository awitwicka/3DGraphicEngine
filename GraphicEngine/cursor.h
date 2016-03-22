#ifndef CURSOR_H
#define CURSOR_H

#include <QVector4D>
#include <QVector>
#include <QMatrix4x4>


class Cursor
{
    float size;
    void InitializeCursor();
    void Clear();
public:
    Cursor();
    QVector<QVector4D> points;
    QVector<QPoint> indices;
    void updateCursor(QMatrix4x4 world);
    float getSize() const;
    void setSize(float value);
    QVector4D center;

};

#endif // CURSOR_H
