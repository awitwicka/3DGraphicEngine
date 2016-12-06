#include "path3c.h"

#include <QFile>
#include <future>
#include <iostream>

Path3C::Path3C()
{

}

Path3C::Path3C(Widget *context)
{
    this->context = context;

    for (int i = 0; i<150; i++) {
        for (int j = 0; j<150; j++)
            heightMap[i][j] = groundLevelWithCutter;
    }
}

void Path3C::PatchSampling(CADSplinePatch* patch, float additionalMat)
{
    QVector4D tmpPos;
    QVector3D Norm;
    QVector3D dv;
    QVector3D du;
    qDebug( "Hello!" );
    for (float u = 0; u <= 1; u+=sampling1) {
        for (float v = 0; v <= 1; v+=sampling1) {
            tmpPos = patch->ComputePos(u, v);

            //scale along normals (add additional material around the object)
            du = QVector3D(patch->ComputeDu(u, v)); //u1' v1
            dv = QVector3D(patch->ComputeDv(u, v)); //u1 v1'
            Norm = QVector3D::crossProduct(du, dv).normalized();
            //add additional material
            tmpPos/=scaleFactor;
            tmpPos += Norm * (additionalMat);

            tmpPos.setZ(tmpPos.z() + offsetX);
            tmpPos.setY(tmpPos.y() + offsetY);

            if (heightMap[(int)tmpPos.z()][(int)tmpPos.y()] < groundLevelWithCutter+tmpPos.x() )
                heightMap[(int)tmpPos.z()][(int)tmpPos.y()] = groundLevelWithCutter+tmpPos.x();
        }
    }
    qDebug( "Bye!" );
}

void Path3C::GeneratePath()
{
    // y
    // |
    // |___ z
    //
    //1500*2 x 1500*2

    float additionalMat = 2 + 8; //additional 2mm and 8mm for cutter    
    CADSplinePatch* patch;
    std::vector<std::future<void>> results;

    for (int i = 0; i< context->SplinePatches.length(); i++) {
        patch = context->SplinePatches[i];
        results.push_back(std::async(std::launch::async, PatchSampling, this, patch, additionalMat)); //void -> return val of a funct
        //PatchSampling(patch, additionalMat);

    }
    for (auto &e : results) {
        e.get();
    }
    //adjust to cutter
    for (int i = 0; i<150; i++) {
        for (int j = 0; j<150; j++)
            heightMap[i][j] -= 8.0f;
    }

    SavePath();
}

void Path3C::SavePath()
{
    qDebug( "GetPatch!" );
    QString path = "E:\\Path.txt";
    QFile file( path );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );

        int precision = 4;
        int count = 3;
        bool isEven = 0;
        bool isEmpty = false;
        bool isPrevEmpty = false;
        bool isNextEmpty = false;
        //TODO: move cutter to start position
        for (int y = 0; y < 150; y+=precision) {
            isEmpty = false;
            isPrevEmpty = false;

            for (int x = 0; x < 149; x+=precision) {
                //check for empty path
                isPrevEmpty = isEmpty;
                isEmpty = isNextEmpty;
                (heightMap[x+1][y] == groundLevel)? isNextEmpty = true : isNextEmpty = false;

                if (!isEmpty || !isPrevEmpty || !isNextEmpty) {
                    if (isEven)
                        stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(heightMap[x][y], 'f', 3) << endl;
                    else
                        stream << "N" << count << "G01" << "X" << QString::number(149-x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(heightMap[149-x][y], 'f', 3) << endl;
                    count++;
                }

            }
            //Add last x
            int x = 149;
            if (isEven)
                stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(heightMap[x][y], 'f', 3) << endl;
            else
                stream << "N" << count << "G01" << "X" << QString::number(149-x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(heightMap[149-x][y], 'f', 3) << endl;
            count++;
            isEven = (!isEven);
            //if (y > 148)
            //    y = 149-precision;
        }
    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
    qDebug( "Done!" );
}

