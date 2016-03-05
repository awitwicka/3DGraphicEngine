#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>
#include "torus.h"

class Widget : public QWidget
{
    Q_OBJECT
    QMatrix4x4 worldMatrix, perspectiveMatrix, viewMatrix;
    qreal scale;
    //Torus t1;
    float Rpersp;
    QPoint savedMouse;
    //QList<CADObject> objects;
    //QVector<QVector4D> points;
    //objs[i]->f(u,v);
    //void DrawClippedLines(QVector4D q1, QPainter painter, QVector4D q2);

public:
    Torus t1;
    explicit Widget(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);

signals:

public slots:
};

#endif // WIDGET_H
