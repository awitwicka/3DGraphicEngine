#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>
#include <QTreeWidget>
#include "torus.h"
#include "marker.h"
#include "cursor.h"
#include "bezier.h"

class Widget : public QWidget
{
    Q_OBJECT
    qreal scale;
    //Torus t1;
    QPoint savedMouse;
    int sceneMode;
    Cursor cursor;
    QColor normalColor;
    QColor highlighColor;
    //QList<CADObject> objects;
    //objs[i]->f(u,v);
    //void DrawCursor

    //TODO use from gui, now function duplicates
    void visitTree(/*QStringList &list*/QList<QTreeWidgetItem*> &items, QTreeWidgetItem *item, QString condition);
    QList<QTreeWidgetItem*> visitTree(QTreeWidget *tree, QString condition);

public:
    //KeyPress(QWidget *parent = 0);
    bool isStereo;
    bool showCurve;
    bool IsMultipleSelect;
    QMatrix4x4 worldMatrix, viewMatrix;
    QList<Marker> markers;
    QList<Bezier> bezier_objects;
    QList<Marker*> selectedMarkers;
    Torus t1;
    QTreeWidget* tree;
    explicit Widget(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
    void switchSceneMode(int index);
    void keyPressEvent(QKeyEvent*event);

    void HandlePointSelection(int i, bool IsMultiSelect);
    void RemovePoint(int i);
    void DeselectPoint();

signals:
    void cursorPosChanged(QVector4D pos, QVector4D posScreen);

public slots:
};

#endif // WIDGET_H
