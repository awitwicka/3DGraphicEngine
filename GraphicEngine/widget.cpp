#include "widget.h"

#include <QPainter>
#include <QWheelEvent>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    //distance between observer and the projection plane
    Rpersp = 200;
    eDistance = 50;
    worldMatrix = QMatrix4x4(); //identity
    perspectiveMatrix = QMatrix4x4(1,0,0,0,
                                   0,1,0,0,
                                   0,0,0,0,
                                   0,0,1/Rpersp,1);
    stereoLMatrix = QMatrix4x4(1,0,-eDistance/(2*Rpersp),0,
                                   0,1,0,0,
                                   0,0,0,0,
                                   0,0,1/Rpersp,1);
    stereoRMatrix = QMatrix4x4(1,0,eDistance/(2*Rpersp),0,
                                   0,1,0,0,
                                   0,0,0,0,
                                   0,0,1/Rpersp,1);
    //TODO: store object on the scene in the QList<CADObject> objects;
    isStereo = false;
    isEditable = false;
    t1 = Torus();
}

//clipping lines going behind observer position and drawing new lines on the scene
void Widget::DrawClippedLines(QPainter &painter, QVector4D q1, QVector4D q2)
{
    QColor color;
    if (q1.z() <= -Rpersp && q2.z() <= -Rpersp) {
       return;//continue;
    }
    if (q1.z() <= -Rpersp && q2.z() > -Rpersp) {
        QVector4D dir = q1-q2;
        QVector4D newq = dir * (-Rpersp+1-q2.z())/dir.z();
        q1 = q2 + newq; //watch not to change w parameter while adding newq
    } else if (q1.z() > -Rpersp && q2.z() <= -Rpersp) {
        QVector4D dir = q2-q1;
        QVector4D newq = dir * (-Rpersp+1-q1.z())/dir.z();
        q2 = q1 + newq;
    }
    if (q1.z() > -Rpersp && q2.z() > -Rpersp) {
        if (isStereo) {
            QVector4D L1  = stereoLMatrix*q1;
            QVector4D L2  = stereoLMatrix*q2;
            QVector4D R1  = stereoRMatrix*q1;
            QVector4D R2  = stereoRMatrix*q2;
            L1 = L1/L1.w();
            L2 = L2/L2.w();
            R1 = R1/R1.w();
            R2 = R2/R2.w();
            painter.setCompositionMode(QPainter::CompositionMode_Plus);
            color.setRgb(150,0,0,255);
            painter.setPen(color);
            painter.drawLine(L1.x(),L1.y(),L2.x(),L2.y());
            color.setRgb(0,0,255,255);
            painter.setPen(color);
            painter.drawLine(R1.x(),R1.y(),R2.x(),R2.y());
        }
        else {
            q1 = perspectiveMatrix*q1;
            q2 = perspectiveMatrix*q2;
            q1 = q1/q1.w();
            q2 = q2/q2.w();
            painter.drawLine(q1.x(),q1.y(),q2.x(),q2.y());
        }
    }
}

void Widget::paintEvent(QPaintEvent *)
{
    //BRUSH AND VIEWPORT SETTINGS
    QPainter painter(this);
    painter.fillRect(this->rect(),Qt::black);
    //QRect r = painter.viewport();
    painter.setViewport(width()/2,height()/2,width(),height());
    painter.setPen(Qt::white);


    //DRAWING OBJECTS ON THE SCENE
    QMatrix4x4 matrix = worldMatrix;
    QColor color;
    //TODO: loop through all existing objects on the scene: for()
    //TODO: draw axis in the middle of the scene
    for (int i = 0; i < t1.indices.length(); i++) {
        QVector4D q1 = t1.points[t1.indices[i].x()]; //TODO: refactoring -> to function
        QVector4D q2 = t1.points[t1.indices[i].y()];  
        q1 = matrix*q1;
        q2 = matrix*q2;
        DrawClippedLines(painter, q1, q2); //TODO: refactor
    }
    //draw points
    for (int i = 0; i < markers.length() ; i++) {
        QVector4D q = markers[i].point;
        q = matrix * q;
        float offset = markers[i].getSize()/2;
        if (q.z() <= -Rpersp) {
           continue;
        } else {
            if (isStereo) {
                QVector4D L  = stereoLMatrix*q;
                QVector4D R  = stereoRMatrix*q;
                L = L/L.w();
                R = R/R.w();
                painter.setCompositionMode(QPainter::CompositionMode_Plus);
                color.setRgb(150,0,0,255);
                painter.fillRect(L.x()-offset, L.y()-offset, offset*2, offset*2, color);
                color.setRgb(0,0,255,255);
                painter.fillRect(R.x()-offset, R.y()-offset, offset*2, offset*2, color);
            }
            else {
                q = perspectiveMatrix*q;
                q = q/q.w();
                painter.fillRect(q.x()-offset, q.y()-offset, offset*2, offset*2, Qt::white);
            }
        }
    }
}



void Widget::wheelEvent(QWheelEvent * event)
{
    worldMatrix.scale(exp(event->delta()/1200.0));
    //qWarning() << "scale:" << 1.0+event->delta()/1200.0;
    update();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    savedMouse = QPoint(event->screenPos().x(), event->screenPos().y());
    switch(isEditable) {
        case true:
            if(event->buttons() & Qt::LeftButton) {


            }
            break;
        case false:
            break;
        default:
            break;
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        float dx = event->screenPos().x() - savedMouse.x();
        float dy = event->screenPos().y() - savedMouse.y();

        QMatrix4x4 translate = QMatrix4x4(1,0,0,dx,
                                          0,1,0,dy,
                                          0,0,1,0,
                                          0,0,0,1);
        QMatrix4x4 translateZ = QMatrix4x4(1,0,0,dx,
                                          0,1,0,0,
                                          0,0,1,dy,
                                          0,0,0,1);

        if (event->modifiers() & Qt::ShiftModifier )
            worldMatrix = translateZ * worldMatrix;
        else
            worldMatrix = translate * worldMatrix;
        savedMouse = QPoint(event->screenPos().x(), event->screenPos().y());
    }
    if(event->buttons() & Qt::RightButton)
    {
        float dx = event->screenPos().x() - savedMouse.x();
        float dy = event->screenPos().y() - savedMouse.y();
        dx /= 100;
        dy /= 100;
        //TODO: correct rotation around z axis
        float dz = dx+dy/2;

        QMatrix4x4 rotX = QMatrix4x4(1,0,0,0,
                                     0,cos(dy),-sin(dy),0,
                                     0,sin(dy),cos(dy),0,
                                     0,0,0,1);
        QMatrix4x4 rotY = QMatrix4x4(cos(dx),0,sin(dx),0,
                                     0,1,0,0,
                                     -sin(dx),0,cos(dx),0,
                                     0,0,0,1);
        QMatrix4x4 rotZ = QMatrix4x4(cos(dz),-sin(dz),0,0,
                                     sin(dz),cos(dz),0,0,
                                     0,0,1,0,
                                     0,0,0,1);

        if (event->modifiers() & Qt::ShiftModifier )
            worldMatrix = rotZ * worldMatrix;
        else
            worldMatrix = rotX * rotY * worldMatrix;
        savedMouse = QPoint(event->screenPos().x(), event->screenPos().y());
    }
    update();
}

void Widget::switchSceneMode(int index) {
    switch(index) {

    case 0:
        isEditable = false;
        break;
    case 1:
        isEditable = true;
        break;
    default:
        break;

    }
}

