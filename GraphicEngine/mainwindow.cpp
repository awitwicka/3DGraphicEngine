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
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0/*t->invisibleRootItem()*/, QStringList(m.name)); //parent, columns names...
    t->addTopLevelItem(item);
    w->update();
}
void MainWindow::on_pushButton_DelMarker_clicked()
{
    //TODO: Delete from list as well
    QList<QTreeWidgetItem*> toDelete = t->selectedItems();
    foreach (QTreeWidgetItem* it, toDelete) {
        t->takeTopLevelItem(t->indexOfTopLevelItem(it));
    }
}

void MainWindow::on_comboBox_activated(int index)
{
    w->switchSceneMode(index);
}
