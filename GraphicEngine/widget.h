#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>
#include "torus.h"
#include "marker.h"

class Widget : public QWidget
{
    Q_OBJECT
    QMatrix4x4 worldMatrix, perspectiveMatrix, stereoLMatrix, stereoRMatrix, viewMatrix;
    qreal scale;
    //Torus t1;
    float Rpersp;
    float eDistance;
    QPoint savedMouse;
    bool isEditable;
    //QList<CADObject> objects;
    //QVector<QVector4D> points;
    //objs[i]->f(u,v);
    void DrawClippedLines(QPainter &painter, QVector4D q1, QVector4D q2);
    //void DrawCursor

public:
    bool isStereo;
    QVector<Marker> markers;
    Torus t1;
    explicit Widget(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
    void switchSceneMode(int index);

signals:

public slots:
};

#endif // WIDGET_H
