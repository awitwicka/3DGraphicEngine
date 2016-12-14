#ifndef PATH3C_H
#define PATH3C_H

#include "widget.h"

#include <QVector3D>
#include "Spline/intersection.h"

class Path3C
{
    //scale of the model on output (ex. 1:6)
    float scaleFactor = 6.0f;//7.2f;
    float offsetX = 80.0f;
    float offsetY = 60.0f;
    float centering = 150/2.0f;

    //accuracy of sampling of parametrisation
    float sampling1 = 0.01f;
    float sampling2 = 0.002f;
    float sampling3 = 0.005f;

    Widget *context;

    void PatchSampling(CADSplinePatch* patch, float additionalMat, float i, float n);
    void PatchSamplingRange(QVector<QVector4D> *result, CADSplinePatch* patch, float additionalMat, float startU, float endU, float startV, float endV);
    void SaveFirstPath();
    //void SavePartialSecPath(float additionalMat, QVector4D Pos, int count, QTextStream stream, float v, bool isFirst, float u, CADSplinePatch* patch, bool isEven);

public slots:
    void doWork();
    void stop();
public:
    //QVector3D materialDimentions = QVector3D(150, 150, 50);

    float materialHeight = 50;
    float heightMap[150][150]; //x y
    float groundLevelWithCutter = 20 + 8;
    float groundLevel = 20;
    float maxZPos = 52;
    
    Path3C();
    Path3C(Widget *context);

    void GeneratePath();
    void GenerateFirstPath();
    void GenerateSecondPath();

private:
    void Scale(float additionalMat, QVector3D Norm, QVector4D &tmpPos);
    QVector<QVector2D> GetClosestVparam(QVector<Intersection*> myIntersections, float v, QVector<bool> isPatch1);
};

#endif // PATH3C_H
