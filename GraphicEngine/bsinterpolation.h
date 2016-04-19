#ifndef BSPLINEINTERPOLATION_H
#define BSPLINEINTERPOLATION_H

#include "cadmarkerobject.h"


class BSInterpolation : public CADMarkerObject
{
    int degree = 3;
    QList<Marker*> DataPoints;
    QList<float> parmeters;
    QList<float> knots;
    void CalculateParameters();
    void CalculateControlPoints();
public:
    BSInterpolation();
    BSInterpolation(QList<Marker*> const & m, QMatrix4x4 matrix);
};

#endif // BSPLINEINTERPOLATION_H
