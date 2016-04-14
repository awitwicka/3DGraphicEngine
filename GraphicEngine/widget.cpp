#include "widget.h"

#include <QPainter>
#include <QWheelEvent>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    worldMatrix = QMatrix4x4(); //identity

    //TODO: store object on the scene in the QList<CADObject> objects;
    isStereo = false;
    sceneMode = 0;
    curveMode = 0;
    t1 = Torus();
    cursor = Cursor();
    //TODO move to point class
    highlighColor = Qt::yellow;
    normalColor = Qt::white;
    IsMultipleSelect = false;
    showCurve = false;
    setFocusPolicy(Qt::StrongFocus);
    selectedVirtualMarker = nullptr;
    //setMouseTracking(true);
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
    //TODO: loop through all existing objects on the scene: for()
    //TODO: draw axis in the middle of the scene
    //t1.Draw(painter, worldMatrix, isStereo);
    cursor.Draw(painter, worldMatrix, isStereo);
    for (int i = 0; i< markers.length(); i++)
        markers[i].Draw(painter, worldMatrix, isStereo);
    for (int i = 0; i< bezier_objects.length(); i++) {
        //TODO: only if zoom points/grabbing/points no change
        bezier_objects[i].InitializeBezier(worldMatrix);
        bezier_objects[i].Draw(painter, worldMatrix, isStereo);
        if (showCurve)
            bezier_objects[i].DrawCurve(painter, worldMatrix, isStereo);

    }
    switch(curveMode) {
        case 0: //b-spline
            for (int i = 0; i< curves.length(); i++) {
                //TODO: only if zoom points/grabbing/points no change
                curves[i].Draw(painter, worldMatrix, isStereo);
                if (showCurve)
                    curves[i].DrawPolygon(painter, worldMatrix, isStereo);
                //here draw virtual bezier points
            }
            break;
        case 1: //bezier
            for (int i = 0; i< curves.length(); i++) {
                //TODO: only if zoom points/grabbing/points no change
                curves[i].Draw(painter, worldMatrix, isStereo);
                for (int j = 0; j < curves[i].bezierMarkers.length(); j++)
                    curves[i].bezierMarkers[j].Draw(painter, worldMatrix, isStereo); ;
                if (showCurve)
                    curves[i].DrawPolygon(painter, worldMatrix, isStereo);
            }
            break;
        default:
            break;
    }
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

void Widget::HandlePointSelection(int i, bool IsMultiSelect)
{ 
    if (selectedVirtualMarker != nullptr) {
        selectedVirtualMarker->setColor(normalColor);
        selectedVirtualMarker->IsSelected = false;
        selectedVirtualMarker = nullptr;
    }

    if (!IsMultiSelect) {
        for (int i = 0; i < selectedMarkers.length(); i++){
            selectedMarkers[i]->setColor(normalColor);
            selectedMarkers[i]->IsSelected = false;
        }
        selectedMarkers.clear();
        selectedMarkers.append(&markers[i]);
        markers[i].setColor(highlighColor);
        markers[i].IsSelected = true;
    } else {
        if (markers[i].IsSelected) {
            selectedMarkers.removeAll(&markers[i]);
            markers[i].setColor(normalColor);
            markers[i].IsSelected = false;

        } else {
            selectedMarkers.append(&markers[i]);
            markers[i].setColor(highlighColor);
            markers[i].IsSelected = true;
        }
    }

    cursor.center = QVector4D(markers[i].point.x(),markers[i].point.y(),markers[i].point.z(),1);
    cursor.updateCursor(worldMatrix);
}

void Widget::RemovePoint(int i)
{
    selectedMarkers.removeAll(&markers[i]);
    markers.removeAt(i);
}

void Widget::DeselectSelectedVirtual()
{
    if (selectedVirtualMarker != nullptr) {
        selectedVirtualMarker->setColor(normalColor);
        selectedVirtualMarker->IsSelected = false;
        selectedVirtualMarker = nullptr;
    }
}

void Widget::SelectVirtualDeselectAll(Marker* m)
{
    for (int i = 0; i < selectedMarkers.length(); i++){
        selectedMarkers[i]->setColor(normalColor);
        selectedMarkers[i]->IsSelected = false;
    }
    selectedMarkers.clear();

    if (selectedVirtualMarker != nullptr) {
        selectedVirtualMarker->setColor(normalColor);
        selectedVirtualMarker->IsSelected = false;
    }
    selectedVirtualMarker = m;
    selectedVirtualMarker->setColor(highlighColor);
    selectedVirtualMarker->IsSelected = true;

    cursor.center = QVector4D(m->point.x(),m->point.y(),m->point.z(),1);
    cursor.updateCursor(worldMatrix);
}

void Widget::SelectIfInRange(QList<Marker> &mark, bool isVirtualMarker)
{
    for (int i = 0; i < mark.length(); i++) {
        float offset = mark[i].getSize()/2;
        float x = savedMouse.x()-width()/2;
        float y = savedMouse.y()-height()/2;
        float worldX = mark[i].pointWorld.x();
        float worldY = mark[i].pointWorld.y();
        if (x >= worldX-offset && x <= worldX+offset && y >= worldY-offset && y <= worldY+offset) {
            if (!isVirtualMarker) {
                HandlePointSelection(i, IsMultipleSelect);
                QList<QTreeWidgetItem*> result = visitTree(tree, mark[i].idname);
                for(int i = 0; i < result.length(); i++)
                    tree->setCurrentItem(result[i]);
            }else
                SelectVirtualDeselectAll(&mark[i]);
            break;
        }
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    savedMouse = QPoint(event->pos().x(), event->pos().y());
    switch(sceneMode) {
        case 0: //Move Scene
            break;
        case 1: //Edit Points
            if(event->buttons() & Qt::LeftButton) {
                    SelectIfInRange(markers, false);
                if (showCurve) {
                    for (int i = 0; i<curves.length(); i++)
                        SelectIfInRange(curves[i].bezierMarkers, true);
                }
            }
            break;
        default:
            break;
    }
    update();
}

void Widget::MovePoints(QMouseEvent *event)
{
    if (selectedMarkers.length() > 0 || selectedVirtualMarker!=nullptr) {
        //only if cursor in range
        /*float dist = sqrt((cursor.center - selectedMarker->point).lengthSquared());
        if (dist > cursor.range)
            return;
        */
        QVector4D oldPos;

        float dx = event->pos().x() - savedMouse.x();
        float dy = event->pos().y() - savedMouse.y();

        if (event->modifiers() & Qt::ShiftModifier ) {
            cursor.center += worldMatrix.inverted()*QVector4D(dx,0,dy,0);
            for (int i = 0; i < selectedMarkers.length(); i++)
                selectedMarkers[i]->point += worldMatrix.inverted()*QVector4D(dx,0,dy,0);
            if (selectedVirtualMarker!=nullptr) {
                oldPos = selectedVirtualMarker->point;
                selectedVirtualMarker->point += worldMatrix.inverted()*QVector4D(dx,0,dy,0); 
            }
        }
        else {
            cursor.center += worldMatrix.inverted()*QVector4D(dx,dy,0,0);
            for (int i = 0; i < selectedMarkers.length(); i++)
                selectedMarkers[i]->point += worldMatrix.inverted()*QVector4D(dx,dy,0,0);
            if (selectedVirtualMarker!=nullptr) {
                oldPos = selectedVirtualMarker->point;
                selectedVirtualMarker->point += worldMatrix.inverted()*QVector4D(dx,dy,0,0);
            }
        }
        savedMouse = QPoint(event->pos().x(), event->pos().y());
        //TODO update only if point a part of any bezier curve //event on pointschange?
        //for(int i = 0; i<bezier_objects.length(); i++)
            //bezier_objects[i].InitializeBezier(worldMatrix);
        //TODO::HERE MOVE BOOR POINTS IN RESPECT TO MOBED BEZIER POINT
        if (selectedVirtualMarker!=nullptr) {
             selectedVirtualMarker->Parent->AdjustOtherPoints(selectedVirtualMarker, oldPos);
        }
        UpdateSceneElements();
    }
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
                MovePoints(event);
            }
            break;
        default:
            break;
    }
    cursor.updateCursor(worldMatrix);
    emit cursorPosChanged(cursor.center, Constants::perspectiveMatrix*worldMatrix*cursor.center);
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
            HandlePointSelection(index, IsMultipleSelect);
            QList<QTreeWidgetItem*> result = visitTree(tree, markers[index].idname);
            for(int i = 0; i < result.length(); i++)
                tree->setCurrentItem(result[i]);
        }
        break;
    default:
        break;
    }
    update();
}

void Widget::switchSceneMode(int index)
{
    sceneMode = index;
}

void Widget::switchCurveMode(int index)
{
    curveMode = index;
    selectedVirtualMarker = nullptr;
    for (int i = 0; i<curves.length(); i++)
        curves[i].InitializeBezierMarkers();
    UpdateSceneElements();
    update();
}

void Widget::UpdateSceneElements()
{
    //TODO upadate only curve that has changed!!!
    //TODO: change to bez only if clicked change/in bezier mode
    switch(curveMode) {
        case 0: //b-spline
            for (int i = 0; i< curves.length(); i++) {
                curves[i].InitializeBSpline(worldMatrix);
            }
            break;
        case 1: //bezier
            for (int i = 0; i< curves.length(); i++) {
                curves[i].ChangeToBezier();
                curves[i].InitializeBezierC2(worldMatrix);
            }   
            break;
        default:
            break;
    }

}

void Widget::visitTree(/*QStringList &list*/QList<QTreeWidgetItem*> &items, QTreeWidgetItem *item, QString condition){
    if (item->text(1) == condition)
        items.append(item);
    //list << item->text(0);
    for(int i=0;i<item->childCount(); ++i)
        visitTree(items, item->child(i), condition);
}

QList<QTreeWidgetItem*> Widget::visitTree(QTreeWidget *tree, QString condition) {
    //QStringList list;
    QList<QTreeWidgetItem*> items;
    for(int i=0;i<tree->topLevelItemCount();++i)
        visitTree(items, tree->topLevelItem(i), condition);
    return items;
}
