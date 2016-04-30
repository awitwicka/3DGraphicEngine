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
    CADSplinePatch* b1 = new BezierPlane(worldMatrix);
    SplinePatches.append(b1);
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
    //TODO: draw axis in the middle of the scene
    //t1.Draw(painter, worldMatrix, isStereo);
    cursor.Draw(painter, worldMatrix, isStereo);
    for (int i = 0; i< markers.length(); i++)
        markers[i].Draw(painter, worldMatrix, isStereo);

    //DRAW SPLINES
    for (int i = 0; i< Splines.length(); i++) {
        //todo move to updatescene and move to CADObject
        //Splines[i]->InitializeSpline(worldMatrix);
        CADObject* obj = dynamic_cast<CADObject*>(Splines[i]);
        obj->Draw(painter, worldMatrix, isStereo);

        if (dynamic_cast<CurveC2*>(Splines[i])) {
        //if(Splines[i]->idname.at(0) == 's') //is BSinterpolation (change to typeof later)
            CurveC2* c = dynamic_cast<CurveC2*>(Splines[i]);
            switch(curveMode) {
                case 0: //b-spline
                    break;
                case 1: //bezier
                    //draw bezier markers
                    for (int j = 0; j < c->bezierMarkers.length(); j++)
                        c->bezierMarkers[j].Draw(painter, worldMatrix, isStereo); ;
                    break;
                default:
                    break;
            }
        }
        if (showCurve)
            Splines[i]->DrawPolygon(painter, worldMatrix, isStereo);
    }
    //DRAW SPLINE_PATCHES
    for (int i = 0; i< SplinePatches.length(); i++) {
        CADObject* obj = dynamic_cast<CADObject*>(SplinePatches[i]);
        obj->Draw(painter, worldMatrix, isStereo);

        for (int j = 0; j<SplinePatches[i]->markers.length(); j++)
            SplinePatches[i]->markers[j].Draw(painter, worldMatrix, isStereo);
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

void Widget::HandlePointSelection(Marker* marker, bool IsMultiSelect)
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
        selectedMarkers.append(marker); //TODO check if marker adress is same in both lists
        marker->setColor(highlighColor);
        marker->IsSelected = true;
    } else {
        if (marker->IsSelected) {
            selectedMarkers.removeAll(marker); //TODO check if marker adress is same in both lists
            marker->setColor(normalColor);
            marker->IsSelected = false;

        } else {
            selectedMarkers.append(marker); //TODO check if marker adress is same in both lists
            marker->setColor(highlighColor);
            marker->IsSelected = true;
        }
    }

    cursor.center = QVector4D(marker->point.x(),marker->point.y(),marker->point.z(),1);
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
                HandlePointSelection(&mark[i], IsMultipleSelect);
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
                for (int i = 0; i<SplinePatches.length(); i++)
                    SelectIfInRange(SplinePatches[i]->markers, false); //TODO Handle deleting points, if points common with Spline Curves
                if (showCurve) {
                    for (int i = 0; i< Splines.length(); i++) {
                        if (dynamic_cast<CurveC2*>(Splines[i])) {
                            CurveC2* c = dynamic_cast<CurveC2*>(Splines[i]);
                            SelectIfInRange(c->bezierMarkers, true);
                        }
                    }
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
        //TODO podpinaj markery do siatki
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
            HandlePointSelection(&markers[index], IsMultipleSelect);
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
    UpdateSceneElements();
    update();
}

void Widget::UpdateSceneElements()
{
    //TODO upadate only curve that has changed!!!
    //TODO: change to bez only if clicked change/in bezier mode
    for (int i = 0; i< Splines.length(); i++) {
        Splines[i]->InitializeSpline(worldMatrix);
        if (dynamic_cast<CurveC2*>(Splines[i])) {
            CurveC2* c = dynamic_cast<CurveC2*>(Splines[i]);
            switch(curveMode) {
                case 0: //b-spline
                    //c->InitializeSpline(worldMatrix); //fill point/indices with bspline
                    break;
                case 1: //bezier
                    c->InitializeBezierC2(worldMatrix);//fill point/indices with bezier
                    break;
                default:
                    break;
            }
        }
    }
    for (int i = 0; i< SplinePatches.length(); i++) {
        SplinePatches[i]->InitializeSpline(worldMatrix);
    }

}

//need to keep pointers while moving so clear the list only on points no change
void Widget::UpdateOnMarkersChanged()
{
    for (int i = 0; i< Splines.length(); i++) {
        Splines[i]->InitializeSpline(worldMatrix);
        if (dynamic_cast<CurveC2*>(Splines[i])) {
            CurveC2* c = dynamic_cast<CurveC2*>(Splines[i]);
            c->InitializeBezierMarkers();
        }
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
