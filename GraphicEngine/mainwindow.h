#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cadloader.h"
#include "path3c.h"
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

    CADLoader loader;
    Path3C path;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void visitTree(QList<QTreeWidgetItem*> &items, QTreeWidgetItem *item, QString condition);
    QList<QTreeWidgetItem*> visitTree(QTreeWidget *tree, QString condition);
    void RemoveRepetitions(QList<QTreeWidgetItem*> toDelete);
    //int FindObjById(QList<CADObject> &list, QString idname);

    int AddMarkerToObjs(QList<QTreeWidgetItem*> objsList, /*QList<CADMarkerObject*> objs,*/ QTreeWidgetItem *item);

private slots:
    void on_spinBox_U_valueChanged(int arg1);

    void on_spinBox_V_valueChanged(int arg1);

    void on_checkBox_stereo_toggled(bool checked);

    void on_pushButton_addMarker_clicked();

    void on_pushButton_DelMarker_clicked();

    void on_pushButton_DelSingleMarker_clicked();

    void on_comboBox_activated(int index);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_widget_cursorPosChanged(const QVector4D &, const QVector4D &);

    void on_checkBox_multiSelect_clicked(bool checked);

    void on_pushButton_addBezier_clicked();

    void on_checkBox_curve_clicked(bool checked);

    void on_checkBox_ppushBezier_clicked(bool checked);

    void on_comboBox_2_activated(int index);

    void on_pushButton_addC0_clicked();

    void on_pushButton_addInterp_clicked();

    void on_comboBox_PatchSelection_activated(int index);

    void on_pushButton_addPatch_clicked();

    void on_spinBoxU_valueChanged(int arg1);

    void on_spinBoxV_valueChanged(int arg1);


    void on_pushButton_Save_clicked();

    void on_pushButton_Open_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Merge_clicked();

    void on_pushButton_fillGap_clicked();

    void on_pushButton_Intersection_clicked();

    void on_pushButton_ScaleUP_clicked();

    void on_pushButton_ScaleDOWN_clicked();

    void on_pushButton_RotX_clicked();

    void on_pushButton_RotY_clicked();

    void on_pushButton_RotZ_clicked();

    void on_pushButton_Front_clicked();

    void on_pushButton_Back_clicked();

    void on_pushButton_Up_clicked();

    void on_pushButton_Down_clicked();

    void on_pushButton_Right_clicked();

    void on_pushButton_Left_clicked();

    void on_pushButton_p1first_clicked();

    void on_spinBox_intersectionStep_valueChanged(int arg1);

    void on_pushButton_all_clicked();

    void on_pushButton_p1sec_clicked();

    void on_pushButton_p2first_clicked();

    void on_pushButton_p2sec_clicked();

    void on_pushButton_GeneratePath_clicked();

private:
    Ui::MainWindow *ui;
    Widget* w;
    QListWidget* l;
    QTreeWidget* t;
    bool isPushBezier;

    void RefreshList();
    Marker* FindMarkerByID(QString id);
    int FindIndexByRef(Marker* marker);
    int FindSurfaceIndexByRef(Marker* marker, QList<Marker*> list);
};

#endif // MAINWINDOW_H
