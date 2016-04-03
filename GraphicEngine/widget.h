#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>
#include "torus.h"
#include "marker.h"
#include "cursor.h"

class Widget : public QWidget
{
    Q_OBJECT
    QMatrix4x4 worldMatrix, viewMatrix;
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

public:
    //KeyPress(QWidget *parent = 0);
    bool isStereo;
    bool IsMultipleSelect;
    QList<Marker> markers;
    QList<Marker*> selectedMarkers;
    Torus t1;
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
