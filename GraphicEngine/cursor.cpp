#include "cursor.h"


Cursor::Cursor()
{
    center = QVector4D(0, 0, 0, 1);
    size = 40;
    range = 40;
    InitializeCursor();
}

QVector<QPoint> Cursor::getIndices() const
{
    return indices;
}

void Cursor::setIndices(const QVector<QPoint> &value)
{
    indices = value;
}

QVector<QVector4D> Cursor::getPoints() const
{
    return points;
}

void Cursor::setPoints(const QVector<QVector4D> &value)
{
    points = value;
}

void Cursor::InitializeCursor()
{
    Clear();
    points.append(QVector4D(center.x()-size/2, center.y(), center.z(), 1));
    points.append(QVector4D(center.x()+size/2, center.y(), center.z(), 1));

    points.append(QVector4D(center.x(), center.y()-size/2, center.z(), 1));
    points.append(QVector4D(center.x(), center.y()+size/2, center.z(), 1));

    points.append(QVector4D(center.x(), center.y(), center.z()-size/2, 1));
    points.append(QVector4D(center.x(), center.y(), center.z()+size/2, 1));

    indices.append(QPoint(0, 1));
    indices.append(QPoint(2, 3));
    indices.append(QPoint(4, 5));
}

void Cursor::updateCursor(QMatrix4x4 world)
{
    Clear();
    QMatrix4x4 inverseWorld = world.inverted();
    points.append(center + inverseWorld * QVector4D(-size/2, 0, 0, 0));
    points.append(center + inverseWorld * QVector4D(+size/2, 0, 0, 0));

    points.append(center + inverseWorld * QVector4D(0, -size/2, 0, 0));
    points.append(center + inverseWorld * QVector4D(0, +size/2, 0, 0));

    points.append(center + inverseWorld * QVector4D(0, 0, -size/2, 0));
    points.append(center + inverseWorld * QVector4D(0, 0, +size/2, 0));

    indices.append(QPoint(0, 1));
    indices.append(QPoint(2, 3));
    indices.append(QPoint(4, 5));
}

void Cursor::Clear()
{
    indices.clear();
    points.clear();
}

float Cursor::getSize() const
{
    return size;
}

void Cursor::setSize(float value)
{
    size = value;
    InitializeCursor();
}

