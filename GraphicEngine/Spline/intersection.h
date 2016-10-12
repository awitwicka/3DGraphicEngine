#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "cadmarkerobject.h"
#include <Patch/cadsplinepatch.h>
#include <QLineSeries>
#include <QChartView>
#include <QMainWindow>

using namespace QtCharts;

class Intersection : public CADObject, public CADMarkerObject
{
    static int id;
    QLineSeries *series1;
    QLineSeries *series2;
    QChart *chart1;
    QChart *chart2;
    QChartView *chartView1;
    QChartView *chartView2;
    QMainWindow window1;
    QMainWindow window2;

    QVector<QVector4D> pointsCurve;
    QVector<QPoint> indicesCurve;

    //forward (1)/backward (-1) direction of finding the next point
    int turn = 1;
    struct UVPointData {
        QVector4D position;
        float u;
        float v;

        UVPointData(QVector4D pos, float u, float v) : position(pos), u(u), v(v) {}
    };

    void Clear();

    double GoalFunction(QVector3D g, QVector3D h, double distDirG, double distDirH);
    void SetGoalFunctionDerivative(QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2, QVector4D &d, double DistDirG, double DistDirH, QVector3D dir);
    QVector3D GetDirection(QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2);

    UVPointData FindClosesPointOnSurface(QVector4D PointPos, CADSplinePatch *patch, double accuracy);
    QVector4D NewtonNextPoint(double e, QVector4D startPoint, CADSplinePatch *patch1, CADSplinePatch *patch2);
    QVector4D GradientDistanceMinimalization(double e, double a, QVector4D x, CADSplinePatch *patch1, CADSplinePatch *patch2);
    QVector4D GradientStep(double e, double a, QVector4D x, CADSplinePatch *patch1, CADSplinePatch *patch2);
    QVector4D GradientNextIntersection(double e, double a, QVector4D x, CADSplinePatch *patch1, CADSplinePatch *patch2);
public:
    float step;
    CADSplinePatch *patch1;
    CADSplinePatch *patch2;
    QVector<QVector4D> UVparameters;
    QVector<QVector2D> UVpatch1;
    QVector<QVector2D> UVpatch2;

    Intersection();
    Intersection(QMatrix4x4 matrix, Marker* start, CADSplinePatch *patch1, CADSplinePatch *patch2, float step);
    ~Intersection();

    void InitializeSpline(QMatrix4x4 matrix);
    void DrawPolygon(QPainter &painter, QMatrix4x4 matrix, bool isStereo);
    void CalculateIntersection(CADSplinePatch *patch2, Marker* start, CADSplinePatch *patch1);
};

#endif // INTERSECTION_H
