#ifndef MARKER_H
#define MARKER_H

#include <QColor>
#include <QPainter>
#include <QVector4D>
#include "cadobject.h"
#include "constants.h"

class CurveC2;

class Marker : public CADObject
{
    static int id;
protected:
    float size;
public:
    //TODO:: pointer on parent
    //QList<CurveC2*> Parents;
    CurveC2* Parent;
    QString name;
    QString idname;
    bool IsSelected;
    QVector4D point; //setter getter virtual
    QVector4D pointWorld;
    Marker* boorParent;

    Marker();
    //~Marker() {};
    Marker(QVector4D position);
    Marker(QVector4D position, QColor color);
    Marker(float x, float y, float z);
    Marker(QVector4D position, QColor color, Marker* parent, Marker* partner, CurveC2* parentCurve);

    void Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo);

    float getSize() const;
    void setSize(float value);
    QColor getColor() const;
    void setColor(const QColor &value);
};

#endif // MARKER_H
