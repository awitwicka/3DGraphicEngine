#include "marker.h"


QColor Marker::getColor() const
{
    return Color;
}

Marker::Marker()
{
    size = 4;
    point = QVector4D(0, 0, 0, 1);
    name = QString("point %1").arg(id);
    idname = QString("%1").arg(id);
    id++;
}

Marker::Marker(float x, float y, float z) : size(10)
{
    name = QString("point %1").arg(id);// + "1");
    idname = QString("%1").arg(id);
    //point(x, y,  z);
    point = QVector4D(x, y, z, 1);
    id++;
}

void Marker::Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    QVector4D q = point;
    q = matrix * q;
    float offset = size/2;

    //calc to get position of point in camera coord
    QVector4D qCamPos = Constants::perspectiveMatrix*q;
    qCamPos = qCamPos/qCamPos.w();
    pointWorld = qCamPos;

    if (q.z() <= -Constants::Rpersp) {
       return;
    } else {
        if (isStereo) {
            QVector4D L  = Constants::stereoLMatrix*q;
            QVector4D R  = Constants::stereoRMatrix*q;
            L = L/L.w();
            R = R/R.w();
            painter.setCompositionMode(QPainter::CompositionMode_Plus);
            painter.fillRect(L.x()-offset, L.y()-offset, offset*2, offset*2, LRedColor);
            painter.fillRect(R.x()-offset, R.y()-offset, offset*2, offset*2, RBlueColor);
        }
        else {
            q = Constants::perspectiveMatrix*q;
            q = q/q.w();
            painter.fillRect(q.x()-offset, q.y()-offset, offset*2, offset*2, Color);
        }
    }

}

float Marker::getSize() const
{
    return size;
}

void Marker::setSize(float value)
{
    size = value;
}

void Marker::setColor(const QColor &value)
{
    Color = value;
}

int Marker::id = 0;
