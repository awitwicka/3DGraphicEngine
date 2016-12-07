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

void Path3C::GeneratePath()
{
    //GenerateFirstPath();
    GenerateSecondPath();
}

void Path3C::GenerateFirstPath()
{
    // y
    // |
    // |___ z
    //
    //1500*2 x 1500*2

    float n = 3; //threads number in each patch
    float additionalMat = 2 + 8; //additional 2mm and 8mm for cutter
    CADSplinePatch* patch;
    std::vector<std::future<void>> results;

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i< context->SplinePatches.length(); i++) {
        for (int j = 0; j<n; j++) {
            patch = context->SplinePatches[i];
            results.push_back(std::async(std::launch::async, PatchSampling, this, patch, additionalMat, j, n)); //void -> return val of a funct
            //PatchSampling(patch, additionalMat);
        }
    }
    for (auto &e : results) {
        e.get();
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration <double, std::milli> (diff).count()/1000 << " s" << std::endl;

    //adjust to cutter
    for (int i = 0; i<150; i++) {
        for (int j = 0; j<150; j++)
            heightMap[i][j] -= 8.0f;
    }
    SaveFirstPath();
}

void Path3C::Scale(float additionalMat, QVector3D Norm, QVector4D &tmpPos)
{
    tmpPos/=scaleFactor;
    tmpPos += Norm * (additionalMat);

    tmpPos.setZ(tmpPos.z() + offsetX);
    tmpPos.setY(tmpPos.y() + offsetY);
}

void Path3C::GenerateSecondPath()
{
    QString path = "E:\\Path2.k12";
    QFile file( path );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );

        QVector4D tmpPos;
        QVector4D Pos;
        QVector3D Norm;
        QVector3D dv;
        QVector3D du;
        CADSplinePatch* patch;
        int count = 3;
        float additionalMat = 6;
        bool isFirst = true;

        for (int i = 0; i< context->SplinePatches.length(); i++) {

            patch = context->SplinePatches[i];
            bool isEven = 0;
            isFirst = true;

            for (float v = 0; v <= 1; v+=sampling1) {
                for (float u = 0; u <= 1; u+=sampling1) {

                    if (isEven) {
                        tmpPos = patch->ComputePos(u, v);
                        if (tmpPos.x() >= 0)
                        {
                            du = QVector3D(patch->ComputeDu(u, v)); //u1' v1
                            dv = QVector3D(patch->ComputeDv(u, v)); //u1 v1'
                            Norm = QVector3D::crossProduct(du, dv).normalized();
                            Scale(additionalMat, Norm, tmpPos);
                            //adjust to cutter
                            tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                        }
                    } else {
                        tmpPos = patch->ComputePos(1-u, v);
                        if (tmpPos.x() >= 0)
                        {
                            du = QVector3D(patch->ComputeDu(1-u, v)); //u1' v1
                            dv = QVector3D(patch->ComputeDv(1-u, v)); //u1 v1'
                            Norm = QVector3D::crossProduct(du, dv).normalized();
                            Scale(additionalMat, Norm, tmpPos);
                            //adjust to cutter
                            tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                        }
                    }

                    if (tmpPos.x() > groundLevel) {
                        if (isFirst){
                            //move to new patch start position
                            stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                            count++;
                            isFirst = false;
                        }
                        stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(tmpPos.x(), 'f', 3) << endl;
                        count++;
                        Pos = tmpPos;
                    }

                }
                isEven = (!isEven);
            }
            //move up after last move
            stream << "N" << count << "G01" << "X" << QString::number(Pos.z()-centering, 'f', 3) << "Y" << QString::number(Pos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
            count++;
        }

    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
}

void Path3C::PatchSampling(CADSplinePatch* patch, float additionalMat, float i, float n)
{
    QVector4D tmpPos;
    QVector3D Norm;
    QVector3D dv;
    QVector3D du;

    float start = i*(1.0f/n);
    float end = (i+1)*(1.0f/n);

    qDebug( "Hello!" );
    for (float u = start; u <= end; u+=sampling2) {
        for (float v = 0; v <= 1; v+=sampling2) {
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

            if (heightMap[(int)tmpPos.z()][(int)tmpPos.y()] < groundLevel+tmpPos.x() )
                heightMap[(int)tmpPos.z()][(int)tmpPos.y()] = groundLevel+tmpPos.x();
        }
    }
    qDebug( "Bye!" );
}

void Path3C::SaveFirstPath()
{
    qDebug( "GetPatch!" );
    QString path = "E:\\Path1.k16";
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
                if (isEven)
                    (heightMap[x+1][y] == groundLevel)? isNextEmpty = true : isNextEmpty = false;
                else
                    (heightMap[149-x-1][y] == groundLevel)? isNextEmpty = true : isNextEmpty = false;

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

