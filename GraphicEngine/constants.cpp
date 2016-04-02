#include "constants.h"

float Constants::Rpersp = 700;
float Constants::eDistance = 50;
QMatrix4x4 Constants::perspectiveMatrix = QMatrix4x4(1,0,0,0,
                               0,1,0,0,
                               0,0,0,0,
                               0,0,1/Rpersp,1);
QMatrix4x4 Constants::stereoLMatrix = QMatrix4x4(1,0,-eDistance/(2*Rpersp),0,
                               0,1,0,0,
                               0,0,0,0,
                               0,0,1/Rpersp,1);
QMatrix4x4 Constants::stereoRMatrix = QMatrix4x4(1,0,eDistance/(2*Rpersp),0,
                               0,1,0,0,
                               0,0,0,0,
                               0,0,1/Rpersp,1);
