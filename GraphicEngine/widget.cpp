#include "widget.h"

#include <QPainter>
#include <QWheelEvent>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    //distance between observer and the projection plane
    Rpersp = 700;
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
    sceneMode = 0;
    t1 = Torus();
    cursor = Cursor();
    highlighColor = Qt::yellow;
    normalColor = Qt::white;
    selectedMarker = nullptr;
    setFocusPolicy(Qt::StrongFocus);
    //setMouseTracking(true);
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
    //cursor
    for (int i = 0; i < cursor.indices.length(); i++) {
        QVector4D q1 = cursor.points[cursor.indices[i].x()]; //TODO: refactoring -> to function
        QVector4D q2 = cursor.points[cursor.indices[i].y()];
        q1 = matrix*q1;
        q2 = matrix*q2;
        DrawClippedLines(painter, q1, q2); //TODO: refactor
    }
    //draw points
    for (int i = 0; i < markers.length() ; i++) {
        QVector4D q = markers[i].point;
        q = matrix * q;
        float offset = markers[i].getSize()/2;

        //calc to get position of point in camera coord
        QVector4D qCamPos = perspectiveMatrix*q;
        qCamPos = qCamPos/qCamPos.w();
        markers[i].pointWorld = qCamPos;

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
                painter.fillRect(q.x()-offset, q.y()-offset, offset*2, offset*2, markers[i].getColor());
            }
        }
    }
    //draw cursor
    /*float offset = cursor.getSize()/2;
    painter.setPen(Qt::red);
    painter.drawLine(cursor.center.x()-offset, cursor.center.y(), cursor.center.x()+offset, cursor.center.y());
    painter.setPen(Qt::green);
    painter.drawLine(cursor.center.x(), cursor.center.y()-offset, cursor.center.x(), cursor.center.y()+offset);
    painter.setPen(Qt::blue);
    painter.drawLine(cursor.center.x()-offset, cursor.center.y(), cursor.center.x()+offset, cursor.center.y());*/
}



void Widget::wheelEvent(QWheelEvent * event)
{
    //qWarning() << "scale:" << 1.0+event->delta()/1200.0;
    switch(sceneMode) {
        case 0: //Move Scene
            worldMatrix.scale(exp(event->delta()/1200.0));
            break;
        case 1: //Edit Points
            break;
        default:
            break;
    }
    cursor.updateCursor(worldMatrix);
    update();
}

void Widget::SelectPoint(int i)
{
    if (selectedMarker != nullptr)
        selectedMarker->setColor(normalColor);
    selectedMarker = &markers[i];
    markers[i].setColor(highlighColor);
    cursor.center = QVector4D(markers[i].point.x(),markers[i].point.y(),markers[i].point.z(),1);
    cursor.updateCursor(worldMatrix);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    savedMouse = QPoint(event->pos().x(), event->pos().y());
    switch(sceneMode) {
        case 0: //Move Scene
            break;
        case 1: //Edit Points
            if(event->buttons() & Qt::LeftButton) {
                for (int i = 0; i < markers.length(); i++) {
                    float offset = markers[i].getSize()/2;
                    float x = savedMouse.x()-width()/2;
                    float y = savedMouse.y()-height()/2;
                    if (x >= markers[i].pointWorld.x()-offset &&
                            x <= markers[i].pointWorld.x()+offset &&
                            y >= markers[i].pointWorld.y()-offset &&
                            y <= markers[i].pointWorld.y()+offset) {
                        SelectPoint(i);
                        //cursor.InitializeCursor();
                        break;
                    }
                }
                //move cursor only if not moving
                //if
            }
            break;
        default:
            break;
    }
    //cursor.updateCursor(worldMatrix);
    update();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{    
    switch(sceneMode) {
        case 0: //Move Scene
            if(event->buttons() & Qt::LeftButton)
            {
                float dx = event->pos().x() - savedMouse.x();
                float dy = event->pos().y() - savedMouse.y();

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
                savedMouse = QPoint(event->pos().x(), event->pos().y());
            }
            if(event->buttons() & Qt::RightButton)
            {
                float dx = -event->pos().x() + savedMouse.x();
                float dy = +event->pos().y() - savedMouse.y();
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
                savedMouse = QPoint(event->pos().x(), event->pos().y());
            }
            break;
        case 1: //Edit Points

            if(event->buttons() & Qt::LeftButton) {
                float dx = event->pos().x() - savedMouse.x();
                float dy = event->pos().y() - savedMouse.y();

                if (event->modifiers() & Qt::ShiftModifier )
                    cursor.center += worldMatrix.inverted()*QVector4D(dx,0,dy,0); //worldMatrix = translateZ * worldMatrix;
                else
                    cursor.center += worldMatrix.inverted()*QVector4D(dx,dy,0,0);//worldMatrix = translate * worldMatrix;

                savedMouse = QPoint(event->pos().x(), event->pos().y());
                //move cursor
            }
            if(event->buttons() & Qt::RightButton) {
                //move point
                if (selectedMarker != nullptr) {
                    float dist = sqrt((cursor.center - selectedMarker->point).lengthSquared());
                    if (dist > cursor.range)
                        return;

                    float dx = event->pos().x() - savedMouse.x();
                    float dy = event->pos().y() - savedMouse.y();

                    if (event->modifiers() & Qt::ShiftModifier ) {
                        cursor.center += worldMatrix.inverted()*QVector4D(dx,0,dy,0);
                        selectedMarker->point += worldMatrix.inverted()*QVector4D(dx,0,dy,0);
                    }
                    else {
                        cursor.center += worldMatrix.inverted()*QVector4D(dx,dy,0,0);
                        selectedMarker->point += worldMatrix.inverted()*QVector4D(dx,dy,0,0);
                    }
                    savedMouse = QPoint(event->pos().x(), event->pos().y());
                }
            }
            break;
        default:
            break;
    }
    cursor.updateCursor(worldMatrix);
    emit cursorPosChanged(cursor.center, perspectiveMatrix*worldMatrix*cursor.center);
    update();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch(sceneMode) {
        case 0: //Move Scene

        break;
    case 1: //Edit Points
        if(event->key() == Qt::Key_Space) {
            float dist; //TODO get init dist
            float index;
            float distTmp;
            if (markers.length() == 0)
                return;

            dist = sqrt((cursor.center - markers[0].point).lengthSquared());
            for (int i = 1; i < markers.length(); i++) {
               distTmp = sqrt((cursor.center - markers[i].point).lengthSquared());
               if (distTmp < dist) {
                   dist = distTmp;
                   index = i;
               }
            }

            if (dist > cursor.range)
                return;
            if (selectedMarker != nullptr)
                selectedMarker->setColor(normalColor);
            selectedMarker = &markers[index];
            markers[index].setColor(highlighColor);
        }
        break;
    default:
        break;
    }
    update();
}

void Widget::switchSceneMode(int index) {
    sceneMode = index;
}

