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

    QList<QString> columns = {"Torus", "tor"};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    t->addTopLevelItem(item);
    //findChild<QSpinBox*>("spinBox_U")->setValue(w.t1.Usegments);
}

MainWindow::~MainWindow()
{
    delete ui;
    //qdeleteall
}

void MainWindow::on_pushButton_clicked()
{
    //findChild<Widget*>()->point.rx() +=10;
    //findChild<Widget*>("widget")->update();
    //QList<Widget*> list = findChildren<Widget*>();
}

void MainWindow::on_spinBox_R_editingFinished()
{
    //float R = findChild<QSpinBox*>("spinBox_R")->value();
    //findChild<Widget*>()->t1.setR(R);
    //findChild<Widget*>()->update();
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

void MainWindow::on_pushButton_addMarker_clicked()
{
    Marker m = Marker(0, 0, 0);
    w->markers.append(m);
    //l->addItem(QListWidgetItem());
    //QList<QTreeWid8getItem *> items;
    QList<QString> columns = {m.name, m.idname};
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(columns)); //parent, columns names...
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    t->addTopLevelItem(item);
    w->update();
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

void MainWindow::on_pushButton_DelMarker_clicked()
{
    //TODO: Delete from list as well
    /*if(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier))
    {
        // Do a few things
    }*/
    QString idname;
    QList<QTreeWidgetItem*> toDelete = t->selectedItems();
    //getrid of repetitions
    int j = 0;
    while(j < toDelete.length()-1) {
        for(int i = j+1; i<toDelete.length(); i++) {
            if (toDelete[j]->text(1) == toDelete[i]->text(1)) {
                toDelete.removeAt(i);
                i--;
            }
        }
        j++;
    }

    foreach (QTreeWidgetItem* it, toDelete) {
        idname = it->text(1);
        //delete marker
        if(!it->parent()) {
            for (int i = 0; i < w->markers.length(); i++) {
                if (w->markers[i].idname == idname)
                    w->RemovePoint(i);
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
    w->update();
}

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
            delete it;
        } else {
            parentIdName = it->parent()->text(1);
            for (int i = 0; i < w->bezier_objects.length(); i++) { //TODO: probably change to CADobject list
                if (w->bezier_objects[i].idname == parentIdName) {
                    for (int j = 0; j < w->bezier_objects[i].markers.length(); j++) {
                        if (w->bezier_objects[i].markers[j]->idname == idname)
                            w->bezier_objects[i].markers.removeAt(j);
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
    w->update();
}

void MainWindow::on_comboBox_activated(int index)
{
    w->switchSceneMode(index);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString idname = item->text(1);
    for (int i = 0; i < w->markers.length(); i++) {
        if (w->markers[i].idname == idname) {
           w->HandlePointSelection(i, w->IsMultipleSelect);
        }
            //w->markers.removeAt(i);
            //function to select marker etc (refactor from switch)
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
    ui->treeWidget->editItem(item, 0);
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

void MainWindow::on_pushButton_addBezier_clicked()
{
    //crete bezier curve
    Bezier b = Bezier(w->selectedMarkers);
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
            if(!it->parent())
                delete it; //delete only if moving from main tree
        }
    }
    t->addTopLevelItem(item);
    w->update();
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
