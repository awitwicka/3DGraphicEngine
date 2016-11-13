#ifndef PATH3C_H
#define PATH3C_H

#include "widget.h"

#include <QVector3D>

class Path3C
{
    Widget *context;
    void SavePath();
public:
    //QVector3D materialDimentions = QVector3D(150, 150, 50);

    //scale of the model on output (ex. 1:20)
    float scaleFactor = 20;
    //accuracy of sampling of parametrisation
    float sampling = 0.1f;

    float materialHeight = 50;
    float heightMap[150][150]; //x y
    float groundLevel = 20;
    
    Path3C();
    Path3C(Widget *context);

    void GeneratePath();

};

#endif // PATH3C_H
