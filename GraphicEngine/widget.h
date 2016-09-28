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
#include "Spline/bezier.h"
#include "Spline/curveC2.h"
#include "Spline/bsinterpolation.h"
#include "Patch/bezierplane.h"
#include "Patch/bsplineplane.h"
#include "Patch/gapfilling.h"
//#include "virtualmarker.h"

class Widget : public QWidget
{
    Q_OBJECT
    qreal scale;
    //Torus t1;
    BezierPlane b1;
    QPoint savedMouse;
    int sceneMode;
    int curveMode;
    QColor normalColor;
    QColor highlighColor;
    QColor background;
    //QList<CADObject> objects;
    //objs[i]->f(u,v);
    //void DrawCursor

    //TODO use from gui, now function duplicates
    void visitTree(/*QStringList &list*/QList<QTreeWidgetItem*> &items, QTreeWidgetItem *item, QString condition);
    QList<QTreeWidgetItem*> visitTree(QTreeWidget *tree, QString condition);

public:
    Cursor cursor;
    Cursor axis;
    //KeyPress(QWidget *parent = 0);
    bool isStereo;
    bool showCurve;
    bool showVectors;
    bool IsMultipleSelect;
    QMatrix4x4 worldMatrix, viewMatrix;
    QList<CADMarkerObject*> Splines;
    QList<CADSplinePatch*> SplinePatches;
    QList<Marker> markers;
    QList<Marker*> selectedMarkers;
    Marker* selectedVirtualMarker;
    Torus t1;

    QTreeWidget* tree;
    explicit Widget(QWidget *parent = 0);

    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
    void keyPressEvent(QKeyEvent*event);

    //points handling
    void HandlePointSelection(Marker* marker, bool IsMultiSelect);
    void RemovePoint(int i);
    void DeselectSelectedVirtual();
    void SelectVirtualDeselectAll(Marker* m);

    void switchSceneMode(int index);
    void switchCurveMode(int index);
    void UpdateSceneElements();
    void UpdateOnMarkersChanged();

    void SelectIfInRange(QList<Marker> & m, bool isVirtualMarker);

    void MovePoints(QMouseEvent *event);

signals:
    void cursorPosChanged(QVector4D pos, QVector4D posScreen);

public slots:
};

#endif // WIDGET_H
