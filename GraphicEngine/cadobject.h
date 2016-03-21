#ifndef CADOBJECT_H
#define CADOBJECT_H

class CADObject{
    public:
        CADObject();
/*
        QVector<QVector4D> points;
        QMatrix4x4 modelMatrix;
        float ulen,vlen;
        virtual QVector4D f(float u, float v)=0;*/
};

/*class Torus:public CADObject{
    //...
    QVector4D f(float u, float v);
};
class BezierPatch:public CADObject{
    //...
    QVector4D f(float u, float v);
};*/

#endif // CADOBJECT_H
