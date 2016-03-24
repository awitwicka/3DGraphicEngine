#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "widget.h"
#include <QListWidget>
#include <QMainWindow>
#include <QTreeWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_spinBox_R_editingFinished();

    void on_spinBox_U_valueChanged(int arg1);

    void on_spinBox_V_valueChanged(int arg1);

    void on_checkBox_stereo_toggled(bool checked);    
    void on_pushButton_addMarker_clicked();

    void on_pushButton_DelMarker_clicked();

    void on_comboBox_activated(int index);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    Widget* w;
    QListWidget* l;
    QTreeWidget* t;
};

#endif // MAINWINDOW_H
