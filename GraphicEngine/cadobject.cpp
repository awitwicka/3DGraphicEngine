#include "cadobject.h"

CADObject::CADObject()
{
    Color = Qt::white;
    LRedColor.setRgb(150,0,0,255);
    RBlueColor.setRgb(0,0,255,255);
}

CADObject::~CADObject()
{

}

void CADObject::Draw(QPainter &painter, QMatrix4x4 matrix, bool isStereo)
{
    for (int i = 0; i < indices.length(); i++) {
        QVector4D q1 = points[indices[i].x()]; //TODO: refactoring -> to function
        QVector4D q2 = points[indices[i].y()];
        q1 = matrix*q1;
        q2 = matrix*q2;
        if (q1.z() <= -Constants::Rpersp && q2.z() <= -Constants::Rpersp) {
           return;//continue;
        }
        if (q1.z() <= -Constants::Rpersp && q2.z() > -Constants::Rpersp) {
            QVector4D dir = q1-q2;
            QVector4D newq = dir * (-Constants::Rpersp+1-q2.z())/dir.z();
            q1 = q2 + newq; //watch not to change w parameter while adding newq
        } else if (q1.z() > -Constants::Rpersp && q2.z() <= -Constants::Rpersp) {
            QVector4D dir = q2-q1;
            QVector4D newq = dir * (-Constants::Rpersp+1-q1.z())/dir.z();
            q2 = q1 + newq;
        }
        if (q1.z() > -Constants::Rpersp && q2.z() > -Constants::Rpersp) {
            if (isStereo) {
                QVector4D L1  = Constants::stereoLMatrix*q1;
                QVector4D L2  = Constants::stereoLMatrix*q2;
                QVector4D R1  = Constants::stereoRMatrix*q1;
                QVector4D R2  = Constants::stereoRMatrix*q2;
                L1 = L1/L1.w();
                L2 = L2/L2.w();
                R1 = R1/R1.w();
                R2 = R2/R2.w();
                painter.setCompositionMode(QPainter::CompositionMode_Plus);
                painter.setPen(LRedColor);
                painter.drawLine(L1.x(),L1.y(),L2.x(),L2.y());
                painter.setPen(RBlueColor);
                painter.drawLine(R1.x(),R1.y(),R2.x(),R2.y());
            }
            else {
                painter.setPen(Color);
                q1 = Constants::perspectiveMatrix*q1;
                q2 = Constants::perspectiveMatrix*q2;
                q1 = q1/q1.w();
                q2 = q2/q2.w();
                painter.drawLine(q1.x(),q1.y(),q2.x(),q2.y());
            }
        }
    }
}


/*QVector4D BezierPatch::f(float u, float v)
{
   // CAdobj* totus=..;

    //(*torus).bbb//cppreference.com
}*/


