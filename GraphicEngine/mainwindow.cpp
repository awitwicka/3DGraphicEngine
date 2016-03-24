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
void MainWindow::on_pushButton_DelMarker_clicked()
{
    //TODO: Delete from list as well
    w->DeselectPoint();
    QString idname;
    QList<QTreeWidgetItem*> toDelete = t->selectedItems();
    foreach (QTreeWidgetItem* it, toDelete) {
        idname = it->text(1);
        t->takeTopLevelItem(t->indexOfTopLevelItem(it));

        for (int i = 0; i < w->markers.length(); i++) {
            if (w->markers[i].idname == idname)
                w->markers.removeAt(i);
        }
        delete it;
        //break;
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
           w->SelectPoint(i);
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
