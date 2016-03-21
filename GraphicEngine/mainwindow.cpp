#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Widget* w = findChild<Widget*>();

    //findChild<QSpinBox*>("spinBox_U")->setValue(w.t1.Usegments);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    findChild<Widget*>()->t1.setU(arg1);
    findChild<Widget*>()->update();
}

void MainWindow::on_spinBox_V_valueChanged(int arg1)
{
    findChild<Widget*>()->t1.setV(arg1);
    findChild<Widget*>()->update();
}

void MainWindow::on_checkBox_stereo_toggled(bool checked)
{
    findChild<Widget*>()->isStereo = checked;
    findChild<Widget*>()->update();
}
