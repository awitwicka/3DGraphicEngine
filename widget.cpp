#include "widget.h"

#include <QPainter>
#include <QWheelEvent>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    //distance between observer and the projection plane
    Rpersp = 200;
    worldMatrix = QMatrix4x4(); //identity
    perspectiveMatrix = QMatrix4x4(1,0,0,0,
                                   0,1,0,0,
                                   0,0,0,0,
                                   0,0,1/Rpersp,1);
    //TODO: store object on the scene in the QList<CADObject> objects;
    t1 = Torus();
    e1 = Elipse();
}

//clipping lines going behind observer position and drawing new lines on the scene
/*void Widget::DrawClippedLines(QPainter painter, QVector4D q1, QVector4D q2)
{
    if (q1.z() <= -Rpersp && q2.z() <= -Rpersp) {
       continue;
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
        q1 = perspectiveMatrix*q1;
        q2 = perspectiveMatrix*q2;
        q1 = q1/q1.w();
        q2 = q2/q2.w();
        painter.drawLine(q1.x(),q1.y(),q2.x(),q2.y());
    }
}*/

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
    //TODO: loop through all existing objects on the scene: for()
    //TODO: draw axis in the middle of the scene
    for (int i = 0; i < t1.indices.length(); i++) {
        QVector4D q1 = t1.points[t1.indices[i].x()]; //TODO: refactoring -> to function
        QVector4D q2 = t1.points[t1.indices[i].y()];  
        q1 = matrix*q1;
        q2 = matrix*q2;
        //DrawClippedLines(painter, q1, q2); //TODO: refactor
        if (q1.z() <= -Rpersp && q2.z() <= -Rpersp) {
           continue;
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
            q1 = perspectiveMatrix*q1;
            q2 = perspectiveMatrix*q2;
            q1 = q1/q1.w();
            q2 = q2/q2.w();
            painter.drawLine(q1.x(),q1.y(),q2.x(),q2.y());
        }
    }

    //DRAWING IMPLICIT ELIPSE WITH RAY CAST
    //widget dimentions
    QPoint topLeft = QPoint(-width()/2,-height()/2);
    QPoint botRight = QPoint(1.5f*width(),1.5f*height());//painter.viewport().bottomRight();
    for (int y = topLeft.y(); y < botRight.y(); y++) {
        for (int x = topLeft.x(); x < botRight.x(); x++) {
            //float woot = e1.f(x, y, matrix);
            //woot = +1;
            if (e1.f(x, y, matrix) != -1) {
               float z1 = sqrt(e1.f(x, y, matrix));
               float z2 = z1*(-1);
               painter.setPen(Qt::yellow);
               painter.drawEllipse(QPoint(x, y), 1, 1);
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

