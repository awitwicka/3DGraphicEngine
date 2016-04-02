#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QMatrix4x4>



class Constants
{
public:
    QMatrix4x4 static perspectiveMatrix;
    QMatrix4x4 static stereoLMatrix;
    QMatrix4x4 static stereoRMatrix;

    //distance between observer and the projection plane
    static float Rpersp;
    //distance between eyes in stareoscopic view
    static float eDistance;
};



#endif // CONSTANTS_H
