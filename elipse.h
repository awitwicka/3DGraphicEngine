#ifndef ELIPSE_H
#define ELIPSE_H

#include <QVector4D>
#include <QMatrix4x4>
#include <QPainter>

class Elipse : public QObject
{
    Q_OBJECT
    float a;
    float b;
    float c;
    QMatrix4x4 D;
    QMatrix4x4 matrix;
    float widgetHeight;
    float widgetWidth;
    void InitializeElipse();
    bool run;
public:
    Elipse();
    //~Elipse();
    float  f(float x, float y, QMatrix4x4 m);
    QVector4D fd(float x, float y, float z, QMatrix4x4 m);
    void setWidgetHeight(float value);
    void setWidgetWidth(float value);
    void setA(float value);
    void setB(float value);
    void setC(float value);
    void setM(const QMatrix4x4 &value);

public slots:
    void doWork();
    void stop();
signals:
    void workFinished(const QImage &output);
    void newApproxReady(const QImage &output);
};

#endif // ELIPSE_H
