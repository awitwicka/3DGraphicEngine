#include "bsinterpolation.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

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
        for (int i = 0; i < w->curves.length(); i++) {
            if (w->curves[i].idname == idname) {
                w->curves[i].markers.append(&w->markers.last());
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
    //create new item
    Marker m = Marker(w->cursor.center);
    w->markers.append(m);
    QList<QString> columns = {m.name, m.idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    int count = 0;
    QString idname;
    QList<QTreeWidgetItem*> selected = t->selectedItems();
    count += AddMarkerToObjs(selected,/* w->curves,*/ item);
    if (count == 0)
        t->addTopLevelItem(item);


    //bezier c2 //todo send signal to redraw given curve
    w->DeselectSelectedVirtual();
    for (int i = 0; i<w->curves.length(); i++)
        w->curves[i].InitializeBezierMarkers();
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
    QString idname;
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
    for (int i = 0; i<w->curves.length(); i++)
        w->curves[i].InitializeBezierMarkers();
    w->update();
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
            for (int i = 0; i < w->markers.length(); i++) {
                if (w->markers[i].idname == idname)
                    w->RemovePoint(i);
            }
            if (w->curves.length() != 0) {
                for (int i = 0; i < w->curves.length(); i++) {
                    if (w->curves[i].idname == idname) {
                        for(int i=0;i<it->childCount(); i++) { //TODO: to use that add with correct parent
                        //for (int i=0; i<w->curves[i].markers.length(); i++) {
                            QTreeWidgetItem * const clone = it->child(i)->clone();
                            QList<QTreeWidgetItem*> result = visitTree(t, it->child(i)->text(1));
                            if (result.length() == 1)
                                t->addTopLevelItem(clone);
                            //delete it->child(i);
                        }
                        delete it;
                        w->curves.removeAt(i);
                    }
                }
            } else
                  delete it;
        } else {
            parentIdName = it->parent()->text(1);
            for (int i = 0; i < w->curves.length(); i++) { //TODO: probably change to CADobject list
                if (w->curves[i].idname == parentIdName) {
                    for (int j = 0; j < w->curves[i].markers.length(); j++) {
                        if (w->curves[i].markers[j]->idname == idname)
                            w->curves[i].markers.removeAt(j);
                    }
                    for (int k = 0; k < w->markers.length(); k++) {
                        if (w->markers[i].idname == idname)
                            w->RemovePoint(i);
                    }
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
    for (int i = 0; i<w->curves.length(); i++)
        w->curves[i].InitializeBezierMarkers();
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
           w->HandlePointSelection(i, w->IsMultipleSelect);
        }
            //w->markers.removeAt(i);
            //function to select marker etc (refactor from switch)
    }
    //add new points
    QString parentidname = item->text(1);
    if (isPushBezier && parentidname.at(0) == 'b') {
        QList<QTreeWidgetItem*> selected = t->selectedItems();
        for (int i = 0; i < w->curves.length(); i++) {
            if (w->curves[i].idname == parentidname) {

                foreach (QTreeWidgetItem* it, selected) {
                    for (int j = 0; j<w->markers.length(); j++) {
                        if (w->markers[j].idname == it->text(1)) {
                            w->curves[i].markers.append(&w->markers[j]);
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
    CurveC2 c = CurveC2(w->selectedMarkers, w->worldMatrix);
    w->curves.append(c);

    //create and add item to list
    QList<QString> columns = {c.name, c.idname};
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
    Bezier b = Bezier(w->selectedMarkers, w->worldMatrix);
    w->bezier_objects.append(b);

    //create and add item to list
    QList<QString> columns = {b.name, b.idname};
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
    BSInterpolation b = BSInterpolation(w->selectedMarkers, w->worldMatrix);
    w->curves_interpolation.append(b);
/*
    //create and add item to list
    QList<QString> columns = {b.name, b.idname};
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
        t->setCurrentItem(item->child(i));*/
    w->update();
}
