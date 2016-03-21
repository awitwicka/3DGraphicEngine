#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMatrix4x4>
#include <QVector4D>
#include <QVector>
#include "torus.h"
#include "elipse.h"

class Widget : public QWidget
{
    Q_OBJECT
    QMatrix4x4 worldMatrix, perspectiveMatrix, viewMatrix;
    qreal scale;
    //Torus t1;
    float Rpersp;
    QPoint savedMouse;
    //QPainter painter;
    QImage image;
    //Elipse
    //TODO: to delete all ABCM
    float A;
    float B;
    float C;
    float M;
    //QList<CADObject> objects;
    //QVector<QVector4D> points;
    //objs[i]->f(u,v);
    //void DrawClippedLines(QVector4D q1, QPainter painter, QVector4D q2);

public:
    Torus t1;
    QThread *thread;
    Elipse *e1;
    void UpdateGui();
    explicit Widget(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);

    void setA(float value);

    void setB(float value);

    void setC(float value);

    void setM(float value);

signals:
    void stop1();
public slots:
    void setImage(const QImage &);
};

#endif // WIDGET_H
