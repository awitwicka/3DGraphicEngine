#include "Spline/bsinterpolation.h"
#include "Patch/bsplineplane.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Spline/intersection.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    w = findChild<Widget*>();
    l = findChild<QListWidget*>();
    t = findChild<QTreeWidget*>();

    //QList<QString> columns = {"Torus", "tor"};
    //QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    //t->addTopLevelItem(item);
    //findChild<QSpinBox*>("spinBox_U")->setValue(w.t1.Usegments);
    w->tree = t;
    isPushBezier = false;
    loader = CADLoader(w);
    path = Path3C(w);
}

MainWindow::~MainWindow()
{
    delete ui;
    //qdeleteall
}

void MainWindow::visitTree(/*QStringList &list*/QList<QTreeWidgetItem*> &items, QTreeWidgetItem *item, QString condition){
    if (item->text(1) == condition)
        items.append(item);
    //list << item->text(0);
    for(int i=0;i<item->childCount(); ++i)
        visitTree(items, item->child(i), condition);
}

QList<QTreeWidgetItem*> MainWindow::visitTree(QTreeWidget *tree, QString condition) {
    //QStringList list;
    QList<QTreeWidgetItem*> items;
    for(int i=0;i<tree->topLevelItemCount();++i)
        visitTree(items, tree->topLevelItem(i), condition);
    return items;
}

void MainWindow::RemoveRepetitions(QList<QTreeWidgetItem*> items)
{
    int j = 0;
    while(j < items.length()-1) {
        for(int i = j+1; i<items.length(); i++) {
            if (items[j]->text(1) == items[i]->text(1)) {
                items.removeAt(i);
                i--;
            }
        }
        j++;
    }
}

void MainWindow::on_spinBox_U_valueChanged(int arg1)
{
    //if (arg >= 1)
    w->t1.setU(arg1);
    w->update();
}

void MainWindow::on_spinBox_V_valueChanged(int arg1)
{
    w->t1.setV(arg1);
    w->update();
}

void MainWindow::on_checkBox_stereo_toggled(bool checked)
{
    w->isStereo = checked;
    w->update();
}

int MainWindow::AddMarkerToObjs(QList<QTreeWidgetItem*> objsList, /*QList<CADMarkerObject*> objs,*/ QTreeWidgetItem *item)
{
    int count = 0;
    foreach (QTreeWidgetItem* it, objsList) {
        QString idname = it->text(1);
        for (int i = 0; i < w->Splines.length(); i++) {
            if (w->Splines[i]->idname == idname) {
                w->Splines[i]->markers.append(&w->markers.last());
                QTreeWidgetItem * const clone = item->clone();
                //QTreeWidgetItem *item = new QTreeWidgetItem(it, QStringList(columns)); //parent, columns names...
                //item->setFlags(item->flags() | Qt::ItemIsEditable);
                it->addChild(clone);
                count++;
            }
        }
    }
    return count;
}

void MainWindow::on_pushButton_addMarker_clicked()
{

    QString idname;
    QList<QTreeWidgetItem*> selected = t->selectedItems();

    //create new item
    Marker m = Marker(w->cursor.center);
    w->markers.append(m);
    QList<QString> columns = {m.name, m.idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    //add marker
    int count = 0;
    count += AddMarkerToObjs(selected,/* w->curves,*/ item);
    if (count == 0)
        t->addTopLevelItem(item);


    //bezier c2 //todo send signal to redraw given curve
    w->DeselectSelectedVirtual();
    w->UpdateOnMarkersChanged();
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_DelMarker_clicked()
{
    //TODO: Delete from list as well
    /*if(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier))
    {
        // Do a few things
    }*/

   /* QString idname;
    QList<QTreeWidgetItem*> toDelete = t->selectedItems();
    RemoveRepetitions(toDelete);

    foreach (QTreeWidgetItem* it, toDelete) {
        idname = it->text(1);
        //delete marker
        if(!it->parent()) {
            for (int i = 0; i < w->markers.length(); i++) {
                if (w->markers[i].idname == idname)
                    w->RemovePoint(i);
            }
            for (int i = 0; i < w->curves.length(); i++) {
                if (w->curves[i].idname == idname) {
                    for(int i=0;i<it->childCount(); ++i) {
                        QTreeWidgetItem * const clone = it->child(i)->clone();
                        QList<QTreeWidgetItem*> result = visitTree(t, it->child(i)->text(1));
                        if (result.length() == 1)
                            t->addTopLevelItem(clone);
                        delete it->child(i);
                    }
                }
            }
            delete it;
        //delete from all instances
        } else {
            QList<QTreeWidgetItem*> result = visitTree(t, idname);
            for (int i = 0; i < w->markers.length(); i++) {
                if (w->markers[i].idname == idname)
                    w->RemovePoint(i);
            }
            foreach (QTreeWidgetItem* t, result) {
                delete t;
            }
        }
    }

    //bezier c2 //todo send signal to redraw given curve
    w->DeselectSelectedVirtual();
    //for (int i = 0; i<w->curves.length(); i++)
    //    w->curves[i].InitializeBezierMarkers();
    w->UpdateSceneElements();
    w->update();*/
}

/*int MainWindow::FindObjById(QList<CADObject> &list, QString idname) {
    for (int i = 0; i < list.length(); i++) {
        if (list[i].idname == idname)
                return i;
    }
    return -1;
}*/

void MainWindow::on_pushButton_DelSingleMarker_clicked()
{
    QString idname;
    QString name;
    QString parentIdName;
    QList<QTreeWidgetItem*> toDelete = t->selectedItems();

    foreach (QTreeWidgetItem* it, toDelete) {
        idname = it->text(1);
        name = it->text(0);
        //delete marker
        if(!it->parent()) {
            if(idname.at(0) == 'g' || idname.at(0) == 'k') {
                delete it;
                for (int i = 0; i < w->SplinePatches.length(); i++) {
                    if (w->SplinePatches[i]->idname == idname)
                        w->SplinePatches.removeAt(i);
                }
            } else {
                for (int i = 0; i < w->markers.length(); i++) {
                    if (w->markers[i].idname == idname)
                        w->RemovePoint(i);
                }
                if (w->Splines.length() != 0) {
                    for (int i = 0; i < w->Splines.length(); i++) {
                        if (w->Splines[i]->idname == idname) {
                            for(int i=0;i<it->childCount(); i++) { //TODO: to use that add with correct parent
                            //for (int i=0; i<w->curves[i].markers.length(); i++) {
                                QTreeWidgetItem * const clone = it->child(i)->clone();
                                QList<QTreeWidgetItem*> result = visitTree(t, it->child(i)->text(1));

                                /*
                                 * int count = 0;
                                for (int r = 0; r<result.length(); r++) {
                                    if (result[r]->parent()->text(1) != it->text(1))
                                          count++;
                                }
                                if (count == 0)
                                 *
                                 */

                                if (result.length() == 1)
                                    t->addTopLevelItem(clone);
                                //delete it->child(i);
                            }
                            delete it;
                            w->Splines.removeAt(i);
                        }
                    }
                } else
                      delete it;
            }
        } else {
            parentIdName = it->parent()->text(1);
            if(parentIdName.at(0) == 'g' || parentIdName.at(0) == 'k')
                return;
            for (int i = 0; i < w->Splines.length(); i++) { //TODO: probably change to CADobject list
                if (w->Splines[i]->idname == parentIdName) {
                    w->Splines[i]->markers.removeAt(it->parent()->indexOfChild((it)));
                }
            }
            delete it;
            //if not in any curve add again to tree as rop item
            QList<QTreeWidgetItem*> result = visitTree(t, idname);
            if (result.length() == 0) {
                QList<QString> columns = {name, idname};
                QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                t->addTopLevelItem(item);
            }
        }
    }

    //bezier c2 //todo send signal to redraw given curve
    w->DeselectSelectedVirtual();
    w->UpdateOnMarkersChanged();
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    //QApplication::mouseButtons().
    //if(event->buttons() & Qt::RightButton) {

    QString idname = item->text(1);
    for (int i = 0; i < w->markers.length(); i++) {
        if (w->markers[i].idname == idname) {
           w->HandlePointSelection(&w->markers[i], w->IsMultipleSelect);
        }
            //w->markers.removeAt(i);
            //function to select marker etc (refactor from switch)
    }
    /*for (int i = 0; i < w->SplinePatches.length(); i++) {
        for (int j=0; j < w->SplinePatches[i]->markers.length(); j++) {
            if (w->SplinePatches[i]->markers[j]->idname == idname) {
                w->HandlePointSelection(w->SplinePatches[i]->markers[j], w->IsMultipleSelect);
            }
        }
    }*/

    //add new points
    QString parentidname = item->text(1);
    if (isPushBezier && parentidname.at(0) == 's') {
        QList<QTreeWidgetItem*> selected = t->selectedItems();
        for (int i = 0; i < w->Splines.length(); i++) {
            if (w->Splines[i]->idname == parentidname) {

                foreach (QTreeWidgetItem* it, selected) {
                    for (int j = 0; j<w->markers.length(); j++) {
                        if (w->markers[j].idname == it->text(1)) {
                            w->Splines[i]->markers.append(&w->markers[j]);
                            QTreeWidgetItem * const clone = it->clone();
                            item->addChild(clone);
                        }

                    }
                    if(!it->parent() && it->text(1).at(0) == 'p') //ok?
                        delete it;
                }
            }
        }
    //lol
    }

    w->update();
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    //TODO: change name of point in model
    /*int index;
    QString idname = item->text(1);
    for (int i = 0; i < w->markers.length(); i++) {
        if (w->markers[i].idname == idname)
            index = i;
    }*/
    t->editItem(item, 0);
}

void MainWindow::on_pushButton_addBezier_clicked()
{
    if (w->selectedMarkers.length() <= 0)
        return;
    //crete bezier curve
    //Bezier b = Bezier(w->selectedMarkers, w->worldMatrix);
    CADMarkerObject* c = new CurveC2(w->selectedMarkers, w->worldMatrix);
    w->Splines.append(c);

    //create and add item to list
    QList<QString> columns = {c->name, c->idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    //swap points so they belong to bezier
    QString idname;
    QList<QTreeWidgetItem*> points = t->selectedItems();
    foreach (QTreeWidgetItem* it, points) {
        idname = it->text(1);
        if (idname.at(0) == 'p') { //check if some other elements besides points selected
            QTreeWidgetItem * const clone = it->clone();
            item->addChild(clone);
            //t->setCurrentItem(item->child(item->childCount()-1));
            if(!it->parent())
                delete it; //delete only if moving from main tree
        }
    }
    t->addTopLevelItem(item);

    //select items under new bezier
    for (int i = 0; i<item->childCount(); i++)
        t->setCurrentItem(item->child(i));
    w->update();
}

void MainWindow::on_widget_cursorPosChanged(const QVector4D &pos, const QVector4D &screen)
{
    //findChild<QLabel*>("label_CursorPos")->setText(QString("x: %1, y: %2, z: %3").arg(pos.x(), pos.y(), pos.z()));
    findChild<QLabel*>("label_CursorPos")->setText(QString("x: %1, y: %2, z: %3").arg((int)pos.x()).arg((int)pos.y()).arg((int)pos.z()));
    findChild<QLabel*>("label_CursorScreen")->setText(QString("x: %1, y: %2").arg((int)screen.x()).arg((int)screen.y()));
}

void MainWindow::on_checkBox_multiSelect_clicked(bool checked)
{
    if(checked) {
        t->setSelectionMode(QAbstractItemView::MultiSelection);
        w->IsMultipleSelect = true;
    }
    if(!checked) {
        t->setSelectionMode(QAbstractItemView::SingleSelection);
        w->IsMultipleSelect = false;
    }
}

void MainWindow::on_checkBox_curve_clicked(bool checked)
{
    if(checked) {
        w->showCurve = true;
    }
    if(!checked) {
        t->setSelectionMode(QAbstractItemView::SingleSelection);
        w->showCurve = false;
    }
    w->update();
}

void MainWindow::on_checkBox_ppushBezier_clicked(bool checked)
{
    if(checked) {
        isPushBezier = true;
    }
    if(!checked) {
        isPushBezier = false;
    }
}

void MainWindow::on_comboBox_activated(int index)
{
    w->switchSceneMode(index);
}

void MainWindow::on_comboBox_2_activated(int index)
{
    w->switchCurveMode(index);
    w->update();
}

void MainWindow::on_pushButton_addC0_clicked()
{
    if (w->selectedMarkers.length() <= 0)
        return;
    //crete bezier curve
    CADMarkerObject* b = new Bezier(w->selectedMarkers, w->worldMatrix);
    w->Splines.append(b);

    //create and add item to list
    QList<QString> columns = {b->name, b->idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    //swap points so they belong to bezier
    QString idname;
    QList<QTreeWidgetItem*> points = t->selectedItems();
    foreach (QTreeWidgetItem* it, points) {
        idname = it->text(1);
        if (idname.at(0) == 'p') { //check if some other elements besides points selected
            QTreeWidgetItem * const clone = it->clone();
            item->addChild(clone);
            //t->setCurrentItem(item->child(item->childCount()-1));
            if(!it->parent())
                delete it; //delete only if moving from main tree
        }
    }
    t->addTopLevelItem(item);

    //select items under new bezier
    for (int i = 0; i<item->childCount(); i++)
        t->setCurrentItem(item->child(i));
    w->update();
}

void MainWindow::on_pushButton_addInterp_clicked()
{
    if (w->selectedMarkers.length() <= 0)
        return;
    //crete bezier curve
    CADMarkerObject* s = new BSInterpolation(w->selectedMarkers, w->worldMatrix);
    w->Splines.append(s);

    //create and add item to list
    QList<QString> columns = {s->name, s->idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    //swap points so they belong to bezier
    QString idname;
    QList<QTreeWidgetItem*> points = t->selectedItems();
    foreach (QTreeWidgetItem* it, points) {
        idname = it->text(1);
        if (idname.at(0) == 'p') { //check if some other elements besides points selected
            QTreeWidgetItem * const clone = it->clone();
            item->addChild(clone);
            //t->setCurrentItem(item->child(item->childCount()-1));
            if(!it->parent())
                delete it; //delete only if moving from main tree
        }
    }
    t->addTopLevelItem(item);

    //select items under new bezier
    for (int i = 0; i<item->childCount(); i++)
        t->setCurrentItem(item->child(i));
    w->update();
}

void MainWindow::on_comboBox_PatchSelection_activated(int index)
{

}

void MainWindow::on_pushButton_addPatch_clicked()
{
    //crete bezier curve
    CADSplinePatch* s;
    if (ui->checkBox_cylinder->isChecked()) {
        if (ui->comboBox_Patch->currentIndex() == 0)
        s = new BezierPlane(w->worldMatrix, &w->markers,(float)ui->spinBoxU->value(), (float)ui->spinBoxV->value(), (float)ui->spinBoxN->value(), (float)ui->spinBoxM->value(), (float)ui->spinBoxR->value(), (float)ui->spinBoxL->value(), w->cursor.center.x(),  w->cursor.center.y(),  w->cursor.center.z(), false);
        if (ui->comboBox_Patch->currentIndex() == 1)
        s = new BSplinePlane(w->worldMatrix, &w->markers, (float)ui->spinBoxU->value(), (float)ui->spinBoxV->value(), (float)ui->spinBoxN->value(), (float)ui->spinBoxM->value(), (float)ui->spinBoxR->value(), (float)ui->spinBoxL->value(), w->cursor.center.x(),  w->cursor.center.y(),  w->cursor.center.z(), false);
    }
    else
    {
        if (ui->comboBox_Patch->currentIndex() == 0)
        s = new BezierPlane(w->worldMatrix, &w->markers, (float)ui->spinBoxU->value(), (float)ui->spinBoxV->value(), (float)ui->spinBoxN->value(), (float)ui->spinBoxM->value(), (float)ui->spinBoxW->value(), (float)ui->spinBoxH->value(), w->cursor.center.x(),  w->cursor.center.y(),  w->cursor.center.z(), true);
        if (ui->comboBox_Patch->currentIndex() == 1)
        s = new BSplinePlane(w->worldMatrix, &w->markers, (float)ui->spinBoxU->value(), (float)ui->spinBoxV->value(), (float)ui->spinBoxN->value(), (float)ui->spinBoxM->value(), (float)ui->spinBoxW->value(), (float)ui->spinBoxH->value(), w->cursor.center.x(),  w->cursor.center.y(),  w->cursor.center.z(), true);
    }
    w->SplinePatches.append(s);

    //create and add item to list
    QList<QString> columns = {s->name, s->idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    //add points to bezier
    for (int i = 0; i<s->markers.length(); i++) {
        columns = {s->markers[i]->name, s->markers[i]->idname};
        QTreeWidgetItem *pkt = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
        pkt->setFlags(pkt->flags() | Qt::ItemIsEditable);
        item->addChild(pkt);
    }

    t->addTopLevelItem(item);
    w->update();
}

void MainWindow::on_spinBoxU_valueChanged(int arg1)
{
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    foreach (QTreeWidgetItem* it, selected) {
        //idname = it->text(1);
        //if (idname.at(0) == 'g' || idname.at(0) == 'k' || idname.at(0) == 'z' || idname.at(0) == 'y') {
            idname = it->text(1);
            //int id = idname.mid(1,1).toInt();
            for (int i = 0; i<w->SplinePatches.length(); i++) {
                if (w->SplinePatches[i]->idname == idname) {
                    //BezierPlane* bp = dynamic_cast<BezierPlane*>(w->SplinePatches[i]);
                    //bp->setU(arg1);
                    w->SplinePatches[i]->setU(arg1);
                }
            }

    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_spinBoxV_valueChanged(int arg1)
{
    //TODO update UV of the selected element in the spinBOX
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        //int id = idname.mid(1,1).toInt();
        for (int i = 0; i<w->SplinePatches.length(); i++) {
            if (w->SplinePatches[i]->idname == idname) {
               // BezierPlane* bp = dynamic_cast<BezierPlane*>(w->SplinePatches[i]);
                //bp->setV(arg1);
                w->SplinePatches[i]->setV(arg1);
            }
        }
    }
    w->UpdateSceneElements();
    w->update();
}

/*
 *
 * idname = it->text(1);
        if (idname.at(0) == 'g') {
            BezierPlane* bp = dynamic_cast<BezierPlane*>(w->SplinePatches[idname.mid(1,1).toInt()]);
            bp->V = arg1;
        }
 * */

void MainWindow::on_pushButton_Save_clicked()
{
    loader.SaveFile();
}

void MainWindow::on_pushButton_Open_clicked()
{
    on_pushButton_Clear_clicked();

    loader.LoadFile();
    RefreshList();
}

void MainWindow::on_pushButton_Clear_clicked()
{
    t->clear();
    w->selectedMarkers.clear();
    w->markers.clear();
    w->SplinePatches.clear();
    w->Splines.clear();
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::RefreshList()
{
    t->clear();
    if (w->markers.isEmpty())
        return;

    //table that shows how many times given markers was used
    int n = w->markers.length();
    int markerCount[n];
    for (int i = 0; i<n; i++)
        markerCount[i] = 0;

    for (int i = 0; i<w->Splines.length(); i++) {
        //create and add item to list
        QList<QString> columns = {w->Splines[i]->name, w->Splines[i]->idname};
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        t->addTopLevelItem(item);

        //add sub markers
        for (int m = 0; m<w->Splines[i]->markers.length(); m++) {
            QList<QString> columns = {w->Splines[i]->markers[m]->name, w->Splines[i]->markers[m]->idname};
            QTreeWidgetItem *mark = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
            mark->setFlags(mark->flags() | Qt::ItemIsEditable);
            item->addChild(mark);

            markerCount[w->markers.indexOf(*w->Splines[i]->markers[m])] += 1;
        }
    }
    for (int i = 0; i<w->SplinePatches.length(); i++) {
        //create and add item to list
        QList<QString> columns = {w->SplinePatches[i]->name, w->SplinePatches[i]->idname};
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        t->addTopLevelItem(item);

        //add sub markers
        for (int m = 0; m<w->SplinePatches[i]->markers.length(); m++) {
            QList<QString> columns = {w->SplinePatches[i]->markers[m]->name, w->SplinePatches[i]->markers[m]->idname};
            QTreeWidgetItem *mark = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
            mark->setFlags(mark->flags() | Qt::ItemIsEditable);
            item->addChild(mark);

            markerCount[w->markers.indexOf(*w->SplinePatches[i]->markers[m])] += 1;
        }
    }
    for (int i =0; i<w->markers.length(); i++) {
        if (markerCount[i] < 1) {
        //create item
            QList<QString> columns = {w->markers[i].name, w->markers[i].idname};
            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(columns)); //parent, columns names...
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            t->addTopLevelItem(item);
        }
    }

    //TODO: select selected markers - test if its working
    for (int i = 0; i<w->selectedMarkers.length(); i++) {
        QList<QTreeWidgetItem*> result = visitTree(t, w->selectedMarkers[i]->idname);
        for(int j = 0; j < result.length(); j++)
            t->setCurrentItem(result[j]);
    }
}

void MainWindow::on_pushButton_fillGap_clicked()
{
    //TODO: prevent error when incorrectly selected things for filing
    QList<CADSplinePatch*> patches;
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    //TODO: SELECT ONLY BEZIER (without bspline)
    //TODO: make a proper id check
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        for (int i = 0; i<w->SplinePatches.length(); i++) {
            if (w->SplinePatches[i]->idname == idname) {
                patches.append(w->SplinePatches[i]);
            }
        }
    }
    w->UpdateSceneElements();
    w->update();
    CADSplinePatch* s = new GapFilling(w->worldMatrix, &w->markers, (float)ui->spinBoxU->value(), (float)ui->spinBoxV->value(), patches[0], patches[1], patches[2]);
    w->SplinePatches.append(s);
    //TODO: create new element on ui?? or just refresh
    RefreshList();
    //GapFilling(patches);
}

void MainWindow::on_pushButton_Intersection_clicked()
{
    Marker* m = nullptr;
    QList<CADSplinePatch*> patches;
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    //TODO optimize search make enum for curves types
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        for (int i = 0; i<w->SplinePatches.length(); i++) {
            if (w->SplinePatches[i]->idname == idname) {
                patches.append(w->SplinePatches[i]);
            }
        }
        for (int i = 0; i<w->markers.length(); i++) {
            if (w->markers[i].idname == idname) {
                m = &w->markers[i];
            }
        }
    }
    if (patches.length()!=2 || m == nullptr)
        return;
    CADMarkerObject* c = new Intersection(w->worldMatrix, m, patches[0], patches[1], (float)ui->spinBox_intersectionStep->value());
    w->Splines.append(c);
    RefreshList();
}

//****************TOOLS********************
void MainWindow::on_pushButton_Merge_clicked()
{
    QList<Marker*> toMerge = QList<Marker*>(w->selectedMarkers);
    //TODO: CLEAR SELECTION ON UI
    w->selectedMarkers.clear();
    //TODO: unselect all selected markers !!!!!!
    if (toMerge.length() <= 1)
        return;

    //get new marker position and add it
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i<toMerge.length(); i++) {
        x+=toMerge[i]->point.x();
        y+=toMerge[i]->point.y();
        z+=toMerge[i]->point.z();
    }
    x/=toMerge.length();
    y/=toMerge.length();
    z/=toMerge.length();
    Marker m = Marker(x,y,z);
    w->markers.append(m);
    //w->markers.append(Marker(x,y,z));
    //QString id = w->markers.last().idname;

    //TODO: UPDATE THE interface ui list

    //update markers in splines
    for (int i = 0; i<toMerge.length(); i++) {

        for (int k = 0; k<w->SplinePatches.length(); k++) {

            w->SplinePatches[k]->ReplaceMarker(toMerge[i], &w->markers[FindIndexByRef(&m)]); //CHECK: try just &m
        }
        //remove marker from the list
        //w->markers.removeAt(FindIndexByRef(toMerge[i])); //TODO make method in widget
        w->markers.removeAll(*toMerge[i]);
    }
    w->UpdateSceneElements();
    w->update();
    RefreshList();
}


void MainWindow::on_pushButton_ScaleUP_clicked()
{
    QList<Marker*> selected = QList<Marker*>(w->selectedMarkers);
    //fnd mid point
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i<selected.length(); i++) {
        x+=selected[i]->point.x();
        y+=selected[i]->point.y();
        z+=selected[i]->point.z();
    }
    x/=selected.length();
    y/=selected.length();
    z/=selected.length();
    QVector4D mid = QVector4D(x, y, z, 1);
    //get vector to each selected point
    for (int i = 0; i<selected.length(); i++) {
        QVector4D v = selected[i]->point - mid;
        selected[i]->point += 10.0f * v.normalized();
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_ScaleDOWN_clicked()
{
    QList<Marker*> selected = QList<Marker*>(w->selectedMarkers);
    //fnd mid point
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i<selected.length(); i++) {
        x+=selected[i]->point.x();
        y+=selected[i]->point.y();
        z+=selected[i]->point.z();
    }
    x/=selected.length();
    y/=selected.length();
    z/=selected.length();
    QVector4D mid = QVector4D(x, y, z, 1);
    //get vector to each selected point
    for (int i = 0; i<selected.length(); i++) {
        QVector4D v = selected[i]->point - mid;
        selected[i]->point -= 10.0f * v.normalized();
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_RotX_clicked()
{
    QList<Marker*> selected = QList<Marker*>(w->selectedMarkers);
    //fnd mid point
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i<selected.length(); i++) {
        x+=selected[i]->point.x();
        y+=selected[i]->point.y();
        z+=selected[i]->point.z();
    }
    x/=selected.length();
    y/=selected.length();
    z/=selected.length();
    QVector4D mid = QVector4D(x, y, z, 1);
    //get vector to each selected point
    for (int i = 0; i<selected.length(); i++) {
        QQuaternion rot = QQuaternion::fromAxisAndAngle(1, 0, 0, 5);

        QVector4D trans = selected[i]->point - mid; //translate to origin
        QVector3D rotated = rot.rotatedVector((QVector3D)trans);
        QVector4D rotated4 = QVector4D(rotated.x(), rotated.y(), rotated.z(), 1);
        rotated4 += mid; //translate back

        selected[i]->point.setX(rotated4.x());
        selected[i]->point.setY(rotated4.y());
        selected[i]->point.setZ(rotated4.z());
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_RotY_clicked()
{
    QList<Marker*> selected = QList<Marker*>(w->selectedMarkers);
    //fnd mid point
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i<selected.length(); i++) {
        x+=selected[i]->point.x();
        y+=selected[i]->point.y();
        z+=selected[i]->point.z();
    }
    x/=selected.length();
    y/=selected.length();
    z/=selected.length();
    QVector4D mid = QVector4D(x, y, z, 1);
    //get vector to each selected point
    for (int i = 0; i<selected.length(); i++) {
        QQuaternion rot = QQuaternion::fromAxisAndAngle(0, 1, 0, 5);

        QVector4D trans = selected[i]->point - mid; //translate to origin
        QVector3D rotated = rot.rotatedVector((QVector3D)trans);
        QVector4D rotated4 = QVector4D(rotated.x(), rotated.y(), rotated.z(), 1);
        rotated4 += mid; //translate back

        selected[i]->point.setX(rotated4.x());
        selected[i]->point.setY(rotated4.y());
        selected[i]->point.setZ(rotated4.z());
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_RotZ_clicked()
{
    QList<Marker*> selected = QList<Marker*>(w->selectedMarkers);
    //fnd mid point
    float x = 0;
    float y = 0;
    float z = 0;
    for (int i = 0; i<selected.length(); i++) {
        x+=selected[i]->point.x();
        y+=selected[i]->point.y();
        z+=selected[i]->point.z();
    }
    x/=selected.length();
    y/=selected.length();
    z/=selected.length();
    QVector4D mid = QVector4D(x, y, z, 1);
    //get vector to each selected point
    for (int i = 0; i<selected.length(); i++) {
        QQuaternion rot = QQuaternion::fromAxisAndAngle(0, 0, 1, 5);

        QVector4D trans = selected[i]->point - mid; //translate to origin
        QVector3D rotated = rot.rotatedVector((QVector3D)trans);
        QVector4D rotated4 = QVector4D(rotated.x(), rotated.y(), rotated.z(), 1);
        rotated4 += mid; //translate back

        selected[i]->point.setX(rotated4.x());
        selected[i]->point.setY(rotated4.y());
        selected[i]->point.setZ(rotated4.z());
    }
    w->UpdateSceneElements();
    w->update();
}


/********************/
int MainWindow::FindIndexByRef(Marker *marker)
{
    QString idname = marker->idname;
    for (int i = 0; i<w->markers.length(); i++) {
        if (w->markers[i].idname == idname)
            return i;
    }
    return -1;
}

int MainWindow::FindSurfaceIndexByRef(Marker *marker, QList<Marker *> list)
{
    QString idname = marker->idname;
    for (int i = 0; i<list.length(); i++) {
        if (list[i]->idname == idname)
            return i;
    }
    return -1;
}

Marker *MainWindow::FindMarkerByID(QString id)
{
    for (int i = 0; i<w->markers.length(); i++) {
        if (w->markers[i].idname == id)
            return &w->markers[i];
    }
    return nullptr;
}

/********************VIEW************************/

void MainWindow::on_pushButton_Front_clicked()
{
    w->worldMatrix = QMatrix4x4();
    w->update();
}

void MainWindow::on_pushButton_Back_clicked()
{
    float dx = 3.14;
    QMatrix4x4 rotY = QMatrix4x4(cos(dx),0,sin(dx),0,
                                 0,1,0,0,
                                 -sin(dx),0,cos(dx),0,
                                 0,0,0,1);
    w->worldMatrix = rotY;
    w->update();
}

void MainWindow::on_pushButton_Up_clicked()
{
    float dy = 3.14/2;
    QMatrix4x4 rotX = QMatrix4x4(1,0,0,0,
                                 0,cos(dy),-sin(dy),0,
                                 0,sin(dy),cos(dy),0,
                                 0,0,0,1);
    w->worldMatrix = rotX;
    w->update();
}

void MainWindow::on_pushButton_Down_clicked()
{
    float dy = -3.14/2;
    QMatrix4x4 rotX = QMatrix4x4(1,0,0,0,
                                 0,cos(dy),-sin(dy),0,
                                 0,sin(dy),cos(dy),0,
                                 0,0,0,1);
    w->worldMatrix = rotX;
    w->update();
}

void MainWindow::on_pushButton_Right_clicked()
{
    float dx = 3.14/2;
    QMatrix4x4 rotY = QMatrix4x4(cos(dx),0,sin(dx),0,
                                 0,1,0,0,
                                 -sin(dx),0,cos(dx),0,
                                 0,0,0,1);
    w->worldMatrix = rotY;
    w->update();
}

void MainWindow::on_pushButton_Left_clicked()
{
    float dx = -3.14/2;
    QMatrix4x4 rotY = QMatrix4x4(cos(dx),0,sin(dx),0,
                                 0,1,0,0,
                                 -sin(dx),0,cos(dx),0,
                                 0,0,0,1);
    w->worldMatrix = rotY;
    w->update();
}

void MainWindow::on_pushButton_p1first_clicked()
{
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    Intersection *intersection = nullptr;
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        if (idname.at(0) == 'x') {
            for (int i = 0; i<w->Splines.length(); i++) {
                if (w->Splines[i]->idname == idname) {
                    intersection = dynamic_cast<Intersection*>(w->Splines[i]);//&w->Splines[i];
                }
            }
        }
        if (intersection != nullptr) {
            intersection->patch2->isTrimmed = false;
            intersection->patch1->isTrimmed = true;
            intersection->patch1->side = true;
            intersection->patch1->isPatch1 = true;
            intersection->patch1->trimData = intersection->UVparameters;
        }
    }
    w->UpdateSceneElements();
    w->update();
    //trim data x y - patch1 u v,   z w - patch2 u v
}

void MainWindow::on_spinBox_intersectionStep_valueChanged(int arg1)
{

}

void MainWindow::on_pushButton_all_clicked()
{
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    Intersection *intersection = nullptr;
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        if (idname.at(0) == 'x') {
            for (int i = 0; i<w->Splines.length(); i++) {
                if (w->Splines[i]->idname == idname) {
                    intersection = dynamic_cast<Intersection*>(w->Splines[i]);//&w->Splines[i];
                }
            }
        }
        if (intersection != nullptr) {
            intersection->patch1->isTrimmed = false;
            intersection->patch2->isTrimmed = false;
        }
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_p1sec_clicked()
{
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    Intersection *intersection = nullptr;
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        if (idname.at(0) == 'x') {
            for (int i = 0; i<w->Splines.length(); i++) {
                if (w->Splines[i]->idname == idname) {
                    intersection = dynamic_cast<Intersection*>(w->Splines[i]);//&w->Splines[i];
                }
            }
        }
        if (intersection != nullptr) {
            intersection->patch2->isTrimmed = false;
            intersection->patch1->isTrimmed = true;
            intersection->patch1->side = false;
            intersection->patch1->isPatch1 = true;
            intersection->patch1->trimData = intersection->UVparameters;
        }
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_p2first_clicked()
{
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    Intersection *intersection = nullptr;
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        if (idname.at(0) == 'x') {
            for (int i = 0; i<w->Splines.length(); i++) {
                if (w->Splines[i]->idname == idname) {
                    intersection = dynamic_cast<Intersection*>(w->Splines[i]);//&w->Splines[i];
                }
            }
        }
        if (intersection != nullptr) {
            intersection->patch1->isTrimmed = false;
            intersection->patch2->isTrimmed = true;
            intersection->patch2->side = true;
            intersection->patch2->isPatch1 = false;
            intersection->patch2->trimData = intersection->UVparameters;
        }
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_p2sec_clicked()
{
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    QString idname;
    Intersection *intersection = nullptr;
    foreach (QTreeWidgetItem* it, selected) {
        idname = it->text(1);
        if (idname.at(0) == 'x') {
            for (int i = 0; i<w->Splines.length(); i++) {
                if (w->Splines[i]->idname == idname) {
                    intersection = dynamic_cast<Intersection*>(w->Splines[i]);//&w->Splines[i];
                }
            }
        }
        if (intersection != nullptr) {
            intersection->patch1->isTrimmed = false;
            intersection->patch2->isTrimmed = true;
            intersection->patch2->side = false;
            intersection->patch2->isPatch1 = false;
            intersection->patch2->trimData = intersection->UVparameters;
        }
    }
    w->UpdateSceneElements();
    w->update();
}

void MainWindow::on_pushButton_GeneratePath_clicked()
{
    path.GeneratePath();
}
