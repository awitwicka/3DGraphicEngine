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

QVector<QVector2D> Path3C::GetClosestVparam(QVector<Intersection*> myIntersections, float v, QVector<bool> isPatch1)
{
    QVector<QVector2D> exclusions;
    QVector4D tmpUV;
    float tmpDiffUV;
    for (int k = 0; k<myIntersections.length(); k++) {
        if (isPatch1[k] == true) {
            tmpDiffUV = abs(myIntersections[k]->UVparameters[0].y() - v);
            tmpUV = myIntersections[k]->UVparameters[0];
        } else if (isPatch1[k] == false) {
            tmpDiffUV = myIntersections[k]->UVparameters[0].w() - v;
            tmpUV = myIntersections[k]->UVparameters[0];
        }
        for (int j = 0; j<myIntersections[k]->UVparameters.length(); j++)
        {
            if (isPatch1[k] == true) {
                if (abs((myIntersections[k]->UVparameters[j].y() - v) < tmpDiffUV) ) //v
                {
                    tmpDiffUV = myIntersections[k]->UVparameters[j].y() - v;
                    tmpUV = myIntersections[k]->UVparameters[j];
                }
            } else if (isPatch1[k] == false) {
                if (abs(myIntersections[k]->UVparameters[j].w() - v < tmpDiffUV) ) //v
                {
                    tmpDiffUV = myIntersections[k]->UVparameters[j].w() - v;
                    tmpUV = myIntersections[k]->UVparameters[j];
                }

            }
        }
        if (isPatch1[k] == true)
            exclusions.push_back(QVector2D(tmpUV.x(), tmpUV.y()));
        else
            exclusions.push_back(QVector2D(tmpUV.z(), tmpUV.w()));
    }
    return exclusions;
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
        QVector<QVector4D> toCheck;
        QVector4D closestPoint;
        bool isTrim = false;

        Intersection* inters;
        QVector<Intersection*> myIntersections;
        QVector<bool> isPatch1;

        QVector<QVector2D> exclusions;
        bool isEnclosed = false;

        CADSplinePatch* patch;
        BSplinePlane* b_patch;
        int count = 3;
        float additionalMat = 6;
        bool isFirst = true;
        bool isFirstTrim = true;
        bool isLastTrim = false;
        bool isEven = 0;
        float e = 0.5; //0.5 mm

        for (int i = 0; i< 1/*context->SplinePatches.length()*/; i++) {

            //add point to chek for trimming
            PatchSamplingRange(toCheck, context->SplinePatches[1], additionalMat, 0, 1, 0, 0.3);//0-0.15
            PatchSamplingRange(toCheck, context->SplinePatches[1], additionalMat, 0, 1, 0.6, 1);//0.75-1
            PatchSamplingRange(toCheck, context->SplinePatches[2], additionalMat, 0, 1, 0.7, 1);//0.9-1

            patch = context->SplinePatches[i];
            b_patch = dynamic_cast<BSplinePlane*>(patch);
            for (auto s : context->Splines) {
                inters = dynamic_cast<Intersection*>(s);
                if (inters->patch1 == patch) //check if not compare by id
                {
                    myIntersections.push_back(inters);
                    isPatch1.push_back(true);
                } else if (inters->patch2 == patch)
                {
                    myIntersections.push_back(inters);
                    isPatch1.push_back(false);
                }
            }

            isEven = 0;
            isFirst = true;

            for (float u = 0; u <= 1; u+=sampling1) {
                for (float v = 0; v <= 1; v+=sampling1) {

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
                        tmpPos = patch->ComputePos(u, 1-v);
                        if (tmpPos.x() >= 0)
                        {
                            du = QVector3D(patch->ComputeDu(u, 1-v)); //u1' v1
                            dv = QVector3D(patch->ComputeDv(u, 1-v)); //u1 v1'
                            Norm = QVector3D::crossProduct(du, dv).normalized();
                            Scale(additionalMat, Norm, tmpPos);
                            //adjust to cutter
                            tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                        }
                    }

                    //finding an intersection point closest to our V -> TODO: check if shouldnt find 2 and make a mean instead
                    //exclusions = GetClosestVparam(myIntersections, v, isPatch1);
                    //isEnclosed = isEnclosed && b_patch->IsEnclosed(u, v, myIntersections[0]->UVparameters, true); //check somehow if true or false

                    isFirstTrim = true;
                    if (tmpPos.x() > groundLevel /*&& u > exclusions[0].x()*/) {
                        //find closest XY
                        isTrim = false;
                        for (int j=0; j<toCheck.length(); j++){
                            if (fabs(toCheck[j].y() - tmpPos.y()) < e && fabs(toCheck[j].z() - tmpPos.z()) < e) {
                                closestPoint = toCheck[j];
                                isTrim = true;
                            }
                        }
                        if (!isTrim || closestPoint.x() < tmpPos.x()) {
                            if (isFirst){
                                //move to new patch start position
                                stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                                count++;
                                isFirst = false;
                            }
                            stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(tmpPos.x(), 'f', 3) << endl;
                            count++;
                            Pos = tmpPos;
                        } else {
                            if (isFirstTrim) {
                                qDebug( "not drawing" );
                                isFirstTrim = false;
                            }
                            stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                            count++;
                        }
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

void Path3C::PatchSamplingRange(QVector<QVector4D> &result, CADSplinePatch *patch, float additionalMat, float startU, float endU, float startV, float endV)
{
    QVector4D tmpPos;
    QVector3D Norm;
    QVector3D dv;
    QVector3D du;

    for (float u = startU; u <= endU; u+=sampling3) {
        for (float v = startV; v <= endV; v+=sampling3) {
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

            tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
            if(tmpPos.x() >= groundLevel ) result.append(tmpPos);
        }
    }
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

