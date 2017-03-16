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
    //GenerateThirdPath();
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
    for (int i = 0; i< /*context->SplinePatches.length()*/3; i++) {
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
    std::cout << "P1: " << std::chrono::duration <double, std::milli> (diff).count()/1000 << " s" << std::endl;

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
    QString path = "E:\\Path3.k08";
    QFile file( path );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );


        QVector4D tmpPos;
        QVector4D Pos;
        QVector3D Norm;
        QVector3D prevNorm;
        QVector3D dv;
        QVector3D du;
        QVector<QVector4D> toCheck;
        QVector4D closestPoint;
        bool isTrim = false;
        float tmpSampling;

        Intersection* inters;
        QVector<Intersection*> myIntersections;
        QVector<bool> isPatch1;

        CADSplinePatch* patch;
        BSplinePlane* b_patch;
        int count = 3;
        float additionalMat = 4;
        bool isFirst = true;
        bool isFirstUnderGround = false;
        bool isEven = 0;
        float sampling = sampling1;
        float e = 0.5; //0.5 mm

        for (int i = 0; i< 3; i++) {

            toCheck.clear();
            std::vector<std::future<void>> results;
            auto start = std::chrono::steady_clock::now();
            //add point to chek for trimming
            if (i == 0) {
                results.push_back(std::async(std::launch::async,
                                             PatchSamplingRange, this, &toCheck, context->SplinePatches[1], additionalMat, 0, 1, 0, 0.3));
                results.push_back(std::async(std::launch::async,
                                             PatchSamplingRange, this, &toCheck, context->SplinePatches[1], additionalMat, 0, 1, 0.6, 1));
                results.push_back(std::async(std::launch::async,
                                             PatchSamplingRange, this, &toCheck, context->SplinePatches[2], additionalMat, 0, 1, 0.7, 1));
            } else if (i == 1) {
                results.push_back(std::async(std::launch::async,
                                             PatchSamplingRange, this, &toCheck, context->SplinePatches[0], additionalMat, 0.1, 0.6, 0.35, 0.9));
                results.push_back(std::async(std::launch::async,
                                             PatchSamplingRange, this, &toCheck, context->SplinePatches[0], additionalMat, 0.1, 0.55, 0, 0.4));
            } else if (i == 2) {
                results.push_back(std::async(std::launch::async,
                                             PatchSamplingRange, this, &toCheck, context->SplinePatches[0], additionalMat, 0.6, 1, 0.55, 1));
            }

            for (auto &e : results) {
                e.get();
            }
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            std::cout << "P3<" << i << ">: " << std::chrono::duration <double, std::milli> (diff).count()/1000 << " s" << std::endl;

            patch = context->SplinePatches[i];
            //for p3 calc
            du = QVector3D(patch->ComputeDu(0, 0)); //u1' v1
            dv = QVector3D(patch->ComputeDv(0, 0)); //u1 v1'
            Norm = QVector3D::crossProduct(du, dv).normalized();


            isEven = true; //false
            isFirst = true;

            if (i == 0) sampling = 0.007;
            if (i == 1) sampling = 0.015;
            if (i == 2) sampling = 0.01;
            tmpSampling = sampling;
            for (float u = 0; u < 1; u+=sampling) {
                for (float v = 0; v < 1; v+=sampling) {


                    //not nice way to avoid drawing 0 points in 1st patch
                    //if (i != 1 || v<1)

                    if (i != 2) {
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
                    } else {
                        if (isEven) {
                            tmpPos = patch->ComputePos(v, u);
                            if (tmpPos.x() >= 0)
                            {
                                du = QVector3D(patch->ComputeDu(v, u-sampling)); //u1' v1
                                dv = QVector3D(patch->ComputeDv(v, u-sampling)); //u1 v1'
                                prevNorm = QVector3D::crossProduct(du, dv).normalized();
                                du = QVector3D(patch->ComputeDu(v, u)); //u1' v1
                                dv = QVector3D(patch->ComputeDv(v, u)); //u1 v1'
                                Norm = QVector3D::crossProduct(du, dv).normalized();

                                Scale(additionalMat, Norm, tmpPos);
                                //adjust to cutter
                                tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                                if (u>0.753 && u<0.613) //0.594 0.713
                                    tmpPos.setX(groundLevel);
                            }
                        } else {
                            tmpPos = patch->ComputePos(1-v, u);
                            if (tmpPos.x() >= 0)
                            {
                                du = QVector3D(patch->ComputeDu(1-v, u-sampling)); //u1' v1
                                dv = QVector3D(patch->ComputeDv(1-v, u-sampling)); //u1 v1'
                                prevNorm = QVector3D::crossProduct(du, dv).normalized();
                                du = QVector3D(patch->ComputeDu(1-v, u)); //u1' v1
                                dv = QVector3D(patch->ComputeDv(1-v, u)); //u1 v1'
                                Norm = QVector3D::crossProduct(du, dv).normalized();

                                Scale(additionalMat, Norm, tmpPos);
                                //adjust to cutter
                                tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                                if (u>0.753 && u<0.613) //0.594 0.713
                                    tmpPos.setX(groundLevel);
                            }
                        }

                        //if (QVector3D::dotProduct(prevNorm, Norm) < 0.866) {//30 deg and bigger
                        //    qDebug() << "u: " << v <<  "v: " << u;
                        //}

                    }
                    //TODO i=2 bigger sampling; check if with isFirstGround we dont omit drawing last points (or if its relevant)
                    //TODO: handle if some points are under ground level in the midle of a path OR correct in faile later
                    if (tmpPos.x() > groundLevel) {
                        //find closest XY
                        closestPoint = toCheck[0];
                        isTrim = false;
                        for (int j=0; j<toCheck.length(); j++){
                            if (fabs(toCheck[j].y() - tmpPos.y()) < e && fabs(toCheck[j].z() - tmpPos.z()) < e
                                    && toCheck[j].x()> tmpPos.x()
                                    && fabs(toCheck[j].y() - tmpPos.y()) + fabs(toCheck[j].z() - tmpPos.z()) < fabs(toCheck[j].y() - closestPoint.y()) + fabs(toCheck[j].z() - closestPoint.z()))
                            {
                                closestPoint = toCheck[j];
                                isTrim = true;
                                //tmpPos.setX(tmpPos.x()+1);
                                //closestPoint.setX(closestPoint.x()+1); //add additional mat at intersections
                            }
                        }
                        if (!isTrim  || closestPoint.x() < tmpPos.x()) {
                            if (isFirst || (isFirstUnderGround)){
                                //move to new patch start position
                                stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                                count++;
                                isFirst = false;
                                isFirstUnderGround = false;
                            }
                            stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(tmpPos.x(), 'f', 3) << endl;
                            count++;
                            Pos = tmpPos;
                            sampling = tmpSampling;
                        } else if (isTrim && i == 0)/*if (i == 0)*/ {
                            stream << "N" <<    count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(closestPoint.x(), 'f', 3) << endl;
                            //stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                            //count++;
                            //sampling = 0.005;

                        }
                        //avoid generating start-end points with path that is completely hidden
                    } else if (tmpPos.x() <= groundLevel && !isFirst && i != 2) {
                        if (!isFirstUnderGround) {
                            stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                            count++;
                            isFirstUnderGround = true;
                        }
                    }

                }
                isEven = (!isEven);
            }
            //move up after last move
            stream << "N" << count << "G01" << "X" << QString::number(Pos.z()-centering, 'f', 3) << "Y" << QString::number(Pos.y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
            count++;
        }
        stream << "*****************" << endl;
        //intersections
        //get intersections
        for (auto s : context->Splines) {
            inters = dynamic_cast<Intersection*>(s);
            myIntersections.push_back(inters);
        }

        QVector3D Norm2;
        QVector4D tmpPos2;
        CADSplinePatch *patch2;
        QVector<QVector4D> points;
        patch = context->SplinePatches[1];
        patch2 = context->SplinePatches[0];
        /*for (int s = 0; s < myIntersections[0]->UVparameters.count(); s++)
        {
                tmpPos = patch->ComputePos( myIntersections[0]->UVparameters[s].z(),  myIntersections[0]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu( myIntersections[0]->UVparameters[s].z(),  myIntersections[0]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv( myIntersections[0]->UVparameters[s].z(),  myIntersections[0]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();

                tmpPos2 = patch2->ComputePos( myIntersections[0]->UVparameters[s].x(),  myIntersections[0]->UVparameters[s].y());
                du = QVector3D(patch2->ComputeDu( myIntersections[0]->UVparameters[s].x(),  myIntersections[0]->UVparameters[s].y()));
                dv = QVector3D(patch2->ComputeDv( myIntersections[0]->UVparameters[s].x(),  myIntersections[0]->UVparameters[s].y()));
                Norm2 = QVector3D::crossProduct(du, dv).normalized();
                Norm = Norm + Norm2;

                Scale(additionalMat, Norm, tmpPos);
                tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                if (tmpPos.x() > groundLevel)
                    points.append(tmpPos);
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[0].z()-centering, 'f', 3) << "Y" << QString::number(points[0].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;
        for (int s = 0; s<points.count(); s++) {
            stream << "N" << count << "G01" << "X" << QString::number(points[s].z()-centering, 'f', 3) << "Y" << QString::number(points[s].y()-centering, 'f', 3) << "Z" << QString::number(points[s].x(), 'f', 3) << endl;
            count++;
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[points.count()-1].z()-centering, 'f', 3) << "Y" << QString::number(points[points.count()-1].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;*/
        //*****************************
        /*points.clear();
        for (int s = 0; s < myIntersections[1]->UVparameters.count(); s++)
        {
                tmpPos = patch->ComputePos( myIntersections[1]->UVparameters[s].z(),  myIntersections[1]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu( myIntersections[1]->UVparameters[s].z(),  myIntersections[1]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv( myIntersections[1]->UVparameters[s].z(),  myIntersections[1]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();

                tmpPos2 = patch2->ComputePos( myIntersections[1]->UVparameters[s].x(),  myIntersections[1]->UVparameters[s].y());
                du = QVector3D(patch2->ComputeDu( myIntersections[1]->UVparameters[s].x(),  myIntersections[1]->UVparameters[s].y()));
                dv = QVector3D(patch2->ComputeDv( myIntersections[1]->UVparameters[s].x(),  myIntersections[1]->UVparameters[s].y()));
                Norm2 = QVector3D::crossProduct(du, dv).normalized();
                Norm = Norm + Norm2;

                Scale(additionalMat, Norm, tmpPos);
                tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                if (tmpPos.x() > groundLevel)
                    points.append(tmpPos);
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[0].z()-centering, 'f', 3) << "Y" << QString::number(points[0].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;
        for (int s = 0; s<points.count(); s++) {
            stream << "N" << count << "G01" << "X" << QString::number(points[s].z()-centering, 'f', 3) << "Y" << QString::number(points[s].y()-centering, 'f', 3) << "Z" << QString::number(points[s].x(), 'f', 3) << endl;
            count++;
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[points.count()-1].z()-centering, 'f', 3) << "Y" << QString::number(points[points.count()-1].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;
        //**************************
        patch = context->SplinePatches[2];
        patch2 = context->SplinePatches[0];
        points.clear();
        for (int s = 0; s < myIntersections[2]->UVparameters.count(); s++)
        {
                tmpPos = patch->ComputePos( myIntersections[2]->UVparameters[s].z(),  myIntersections[2]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu( myIntersections[2]->UVparameters[s].z(),  myIntersections[2]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv( myIntersections[2]->UVparameters[s].z(),  myIntersections[2]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();

                tmpPos2 = patch2->ComputePos( myIntersections[2]->UVparameters[s].x(),  myIntersections[2]->UVparameters[s].y());
                du = QVector3D(patch2->ComputeDu( myIntersections[2]->UVparameters[s].x(),  myIntersections[2]->UVparameters[s].y()));
                dv = QVector3D(patch2->ComputeDv( myIntersections[2]->UVparameters[s].x(),  myIntersections[2]->UVparameters[s].y()));
                Norm2 = QVector3D::crossProduct(du, dv).normalized();
                Norm = Norm + Norm2;

                Scale(additionalMat, Norm, tmpPos);
                tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                if (tmpPos.x() > groundLevel)
                    points.append(tmpPos);
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[0].z()-centering, 'f', 3) << "Y" << QString::number(points[0].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;
        for (int s = 0; s<points.count(); s++) {
            stream << "N" << count << "G01" << "X" << QString::number(points[s].z()-centering, 'f', 3) << "Y" << QString::number(points[s].y()-centering, 'f', 3) << "Z" << QString::number(points[s].x(), 'f', 3) << endl;
            count++;
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[points.count()-1].z()-centering, 'f', 3) << "Y" << QString::number(points[points.count()-1].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;*/
        //********************obwiednia     


        points.clear();
        QVector<QVector4D> leftArm;
        QVector<QVector4D> rightArm;
        int c = myIntersections[10]->UVparameters.count();
        for (int i = 0; i<myIntersections[10]->UVparameters.count(); i++)
            if (i < c/2) rightArm.append(myIntersections[10]->UVparameters[i]);
        for (int i = 0; i<myIntersections[10]->UVparameters.count(); i++)
            if (i > c/2) leftArm.append(myIntersections[10]->UVparameters[i]);

        patch = context->SplinePatches[0];
        QVector<QVector4D> p0 = GetPositions(myIntersections[8]->UVparameters, patch, additionalMat);
        patch = context->SplinePatches[1];
        QVector<QVector4D> pLA = GetPositions(leftArm, patch, additionalMat);
        patch = context->SplinePatches[1];
        QVector<QVector4D> pRA = GetPositions(rightArm, patch, additionalMat);

        QVector2D i3a = FindIntersectionOfIntersections(pRA, p0);
        QVector2D i3b = FindIntersectionOfIntersections(pLA, p0);
        patch = context->SplinePatches[1];
        for (int s = i3b.x(); s < pLA.count()+i3a.x()/*myIntersections[10]->UVparameters.count()*/; s++)
        {
            //if (myIntersections[10]->UVparameters[s].x() < myIntersections[10]->UVparameters[i3a.x()].x()
            //        && myIntersections[10]->UVparameters[s].x() > myIntersections[10]->UVparameters[i3b.x()].x()) //>=?

            //{
                tmpPos = patch->ComputePos(myIntersections[10]->UVparameters[s].z(), myIntersections[10]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[10]->UVparameters[s].z(), myIntersections[10]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[10]->UVparameters[s].z(), myIntersections[10]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                points.append(tmpPos);
            //}
        }
        patch = context->SplinePatches[0];
        for (int s = 0; s < myIntersections[8]->UVparameters.count(); s++)
        {
            if (myIntersections[8]->UVparameters[s].x() < myIntersections[8]->UVparameters[i3a.y()].x()
                    && myIntersections[8]->UVparameters[s].x() > myIntersections[8]->UVparameters[i3b.y()].x()) //>=?
            {
                tmpPos = patch->ComputePos(myIntersections[8]->UVparameters[s].z(), myIntersections[8]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[8]->UVparameters[s].z(), myIntersections[8]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[8]->UVparameters[s].z(), myIntersections[8]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                points.append(tmpPos);
            }
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[0].z()-centering, 'f', 3) << "Y" << QString::number(points[0].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;
        for (int s = 0; s<points.count(); s++) {
            stream << "N" << count << "G01" << "X" << QString::number(points[s].z()-centering, 'f', 3) << "Y" << QString::number(points[s].y()-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
            count++;
        }
        stream << "N" << count << "G01" << "X" << QString::number(points[points.count()-1].z()-centering, 'f', 3) << "Y" << QString::number(points[points.count()-1].y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        count++;

        QVector<QVector4D> kratka;
        //kratka
        int maxX = points[0].z();
        int minX = points[0].z();
        int maxY = points[0].y();
        int minY = points[0].y();
        for (int i = 0; i<points.count(); i++) {
            if(points[i].z()>maxX) maxX=points[i].z();
            if(points[i].z()<minX) minX=points[i].z();
            if(points[i].y()>maxY) maxY=points[i].y();
            if(points[i].y()<minY) minY=points[i].y();
        }
        isFirst = false;
        for (int y = minY; y < maxY; y++) {
            for (int x = minX; x < maxX; x++)
            {
                if (isEnclosed(points, x, y)) {
                    stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                    count++;
                } else {
                    stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                }

            }

        }
        for (int x = minX; x < maxX; x++) {
            for (int y = minY; y < maxY; y++)
            {
                if (isEnclosed(points, x, y)) {
                    stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                    count++;
                } else {
                    stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
                }

            }

        }
    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
}

void Path3C::GenerateThirdPath()
{
    QString path = "E:\\Path2.f10";
    QFile file( path );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );

        Intersection* inters;
        QVector<Intersection*> myIntersections;
        
        float additionalMat = 5;
        int precision = 5;
        int count = 3;
        bool isEven = true;
        int x;
        int y;

        float firstNumerZPos = 0;
        //UP
        stream << "N" << count << "G01" << "X" << QString::number(-7-centering, 'f', 3) << "Y" << QString::number(-13-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        stream << "N" << count << "G01" << "X" << QString::number(-7-centering, 'f', 3) << "Y" << QString::number(-13-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
        for (y = 0; y < 150; y+=precision)
        {
            x = 0;
            firstNumerZPos = groundLevel;
            while (firstNumerZPos == groundLevel && x<150)
            {
                if (heightMap[x][y] != groundLevel)
                    firstNumerZPos = heightMap[x][y];
                x++;
            }
            //when model in between
            if (x<150) {
                 if (isEven) {
                     stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                     stream << "N" << count << "G01" << "X" << QString::number(x-1-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                 }
                 else
                 {
                     stream << "N" << count << "G01" << "X" << QString::number(x-1-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                     stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                 }
                 count++;
            }
            //when empty path
            else
            {
                if (isEven)
                {
                    stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                    stream << "N" << count << "G01" << "X" << QString::number(149-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                }
                else
                {
                    stream << "N" << count << "G01" << "X" << QString::number(149-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                    stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                }
            }
            isEven = (!isEven);
        }
        //cutter is at x = 0
        //UP
        //stream << "N" << count << "G01" << "X" << QString::number(x-1-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        //UP
        //stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(149-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        isEven = false; //?
        for (y = 149; y >= 0; y-=precision)
        {
            x = 149;
            firstNumerZPos = groundLevel;
            while (firstNumerZPos == groundLevel && x>=0)
            {
                if (heightMap[x][y] != groundLevel)
                    firstNumerZPos = heightMap[x][y];
                x--;
            }
            //when model in between
            if (x<150) {
                 if (isEven) {
                     stream << "N" << count << "G01" << "X" << QString::number(149-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                     stream << "N" << count << "G01" << "X" << QString::number(x+1-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                 }
                 else
                 {
                     stream << "N" << count << "G01" << "X" << QString::number(x+1-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                     stream << "N" << count << "G01" << "X" << QString::number(149-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                 }
                 count++;
            }
            //when empty path
            else
            {
                if (isEven)
                {
                    stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                    stream << "N" << count << "G01" << "X" << QString::number(149-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                }
                else
                {
                    stream << "N" << count << "G01" << "X" << QString::number(149-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                    stream << "N" << count << "G01" << "X" << QString::number(0-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
                }
            }
            isEven = (!isEven);
        }
        //UP
        stream << "N" << count << "G01" << "X" << QString::number(x-centering, 'f', 3) << "Y" << QString::number(y-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;

        //******** OBWIEDNIA ************''
        //finding intersection points
        //get intersections
        //p2 - correct patch z w
        //p1 - common patch for comparison x y

        QVector4D tmpPos;
        QVector3D Norm;
        QVector3D dv;
        QVector3D du;

        CADSplinePatch* patch;
        QVector<QVector4D> result;
        for (int s = 3; s < 10; s++) {
            inters = dynamic_cast<Intersection*>(context->Splines[s]);
            myIntersections.push_back(inters);
        }
        //*******UPPER PART
        QVector<QVector4D> leftArm;
        QVector<QVector4D> rightArm;
        int c = myIntersections[6]->UVparameters.count();
        for (int i = 0; i<myIntersections[6]->UVparameters.count(); i++)
            if (i < c/2) rightArm.append(myIntersections[6]->UVparameters[i]);
        for (int i = 0; i<myIntersections[6]->UVparameters.count(); i++)
            if (i > c/2) leftArm.append(myIntersections[6]->UVparameters[i]);



        patch = context->SplinePatches[0];
        QVector<QVector4D> p0 = GetPositions(myIntersections[5]->UVparameters, patch, additionalMat);
        patch = context->SplinePatches[1];
        QVector<QVector4D> pLA = GetPositions(leftArm, patch, additionalMat);
        patch = context->SplinePatches[1];
        QVector<QVector4D> pRA = GetPositions(rightArm, patch, additionalMat);

        QVector2D i3a = FindIntersectionOfIntersections(pRA, p0);
        QVector2D i3b = FindIntersectionOfIntersections(pLA, p0);

        //UP
        patch = context->SplinePatches[1];
        tmpPos = patch->ComputePos(rightArm[i3a.x()].z(), rightArm[i3a.x()].w());
        du = QVector3D(patch->ComputeDu(rightArm[i3a.x()].z(), rightArm[i3a.x()].w()));
        dv = QVector3D(patch->ComputeDv(rightArm[i3a.x()].z(), rightArm[i3a.x()].w()));
        Norm = QVector3D::crossProduct(du, dv).normalized();
        Scale(additionalMat, Norm, tmpPos);
        stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()+8-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-8-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;
        stream << "N" << count << "G01" << "X" << QString::number(tmpPos.z()+8-centering, 'f', 3) << "Y" << QString::number(tmpPos.y()-8-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;

        for (int s = 0; s < rightArm.count(); s++)
        {
            if (rightArm[s].x() > rightArm[i3a.x()].x()) //>=?
            {
                tmpPos = patch->ComputePos(rightArm[s].z(), rightArm[s].w());
                du = QVector3D(patch->ComputeDu(rightArm[s].z(), rightArm[s].w()));
                dv = QVector3D(patch->ComputeDv(rightArm[s].z(), rightArm[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                result.append(tmpPos);
            }
        }
        for (int s = 0; s < leftArm.count(); s++)
        {
            if (leftArm[s].x() < leftArm[i3b.x()].x()) //>=?
            {
                tmpPos = patch->ComputePos(leftArm[s].z(), leftArm[s].w());
                du = QVector3D(patch->ComputeDu(leftArm[s].z(), leftArm[s].w()));
                dv = QVector3D(patch->ComputeDv(leftArm[s].z(), leftArm[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                result.append(tmpPos);
            }
        }
        patch = context->SplinePatches[0];
        for (int s = 0; s < myIntersections[5]->UVparameters.count(); s++)
        {
            if (myIntersections[5]->UVparameters[s].x() > myIntersections[5]->UVparameters[i3b.y()].x()) //>=?
            {
                tmpPos = patch->ComputePos(myIntersections[5]->UVparameters[s].z(), myIntersections[5]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[5]->UVparameters[s].z(), myIntersections[5]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[5]->UVparameters[s].z(), myIntersections[5]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                result.append(tmpPos);
            }
        }


        //intersections LOWER PART
        patch = context->SplinePatches[0];
        p0 = GetPositions(myIntersections[4]->UVparameters, patch, additionalMat);
        patch = context->SplinePatches[2];
        QVector<QVector4D> p2a = GetPositions(myIntersections[0]->UVparameters, patch, additionalMat);
        patch = context->SplinePatches[2];
        QVector<QVector4D> p2b = GetPositions(myIntersections[3]->UVparameters, patch, additionalMat);

        QVector2D i1 = FindIntersectionOfIntersections(p2a, p0);
        QVector2D i2 = FindIntersectionOfIntersections(p2b, p0);
        patch = context->SplinePatches[0];
        for (int s = 0; s < myIntersections[4]->UVparameters.count(); s++)
        {
            //v
            if (myIntersections[4]->UVparameters[s].x() > myIntersections[4]->UVparameters[i1.y()].x()) //>=?
            {
                tmpPos = patch->ComputePos(myIntersections[4]->UVparameters[s].z(), myIntersections[4]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[4]->UVparameters[s].z(), myIntersections[4]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[4]->UVparameters[s].z(), myIntersections[4]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                //tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                result.append(tmpPos);
            }
        }
        result.removeLast();
        /*result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();
        result.removeLast();*/
        //*******
        patch = context->SplinePatches[2];
        for (int s = 0; s < myIntersections[0]->UVparameters.count(); s++)
        {
            //v
            if (myIntersections[0]->UVparameters[s].y() > myIntersections[0]->UVparameters[i1.x()].y()) //>=?
            {
                tmpPos = patch->ComputePos(myIntersections[0]->UVparameters[s].z(), myIntersections[0]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[0]->UVparameters[s].z(), myIntersections[0]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[0]->UVparameters[s].z(), myIntersections[0]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                //tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                result.append(tmpPos);
            }
        }
        for (int s = 0; s < myIntersections[1]->UVparameters.count(); s++) {
            tmpPos = patch->ComputePos(myIntersections[1]->UVparameters[s].z(), myIntersections[1]->UVparameters[s].w());
            du = QVector3D(patch->ComputeDu(myIntersections[1]->UVparameters[s].z(), myIntersections[1]->UVparameters[s].w()));
            dv = QVector3D(patch->ComputeDv(myIntersections[1]->UVparameters[s].z(), myIntersections[1]->UVparameters[s].w()));
            Norm = QVector3D::crossProduct(du, dv).normalized();
            Scale(additionalMat, Norm, tmpPos);
            //tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
            result.append(tmpPos);
        }
        for (int s = 0; s < myIntersections[2]->UVparameters.count(); s++)
        {
            tmpPos = patch->ComputePos(myIntersections[2]->UVparameters[s].z(), myIntersections[2]->UVparameters[s].w());
            du = QVector3D(patch->ComputeDu(myIntersections[2]->UVparameters[s].z(), myIntersections[2]->UVparameters[s].w()));
            dv = QVector3D(patch->ComputeDv(myIntersections[2]->UVparameters[s].z(), myIntersections[2]->UVparameters[s].w()));
            Norm = QVector3D::crossProduct(du, dv).normalized();
            Scale(additionalMat, Norm, tmpPos);
            //tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
            result.append(tmpPos);
        }
        for (int s = 0; s < myIntersections[3]->UVparameters.count(); s++)
        {
            //v
            if (myIntersections[3]->UVparameters[s].y() > myIntersections[3]->UVparameters[i2.x()].y()) //>=?
            {
                tmpPos = patch->ComputePos(myIntersections[3]->UVparameters[s].z(), myIntersections[3]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[3]->UVparameters[s].z(), myIntersections[3]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[3]->UVparameters[s].z(), myIntersections[3]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                //tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                result.append(tmpPos);
            }
        }
        //*******
        int calc = 0;
        patch = context->SplinePatches[0];
        for (int s = 0; s < myIntersections[4]->UVparameters.count(); s++)
        {
            //v
            if (myIntersections[4]->UVparameters[s].x() < myIntersections[4]->UVparameters[i2.y()].x()) //>=?
            {
                c++;
                tmpPos = patch->ComputePos(myIntersections[4]->UVparameters[s].z(), myIntersections[4]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[4]->UVparameters[s].z(), myIntersections[4]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[4]->UVparameters[s].z(), myIntersections[4]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                //tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
                result.append(tmpPos);
            }
            if (calc == 1)
            {
                result.removeLast();
            }
        }
        //*****UPPER
        for (int s = 0; s < myIntersections[5]->UVparameters.count(); s++)
        {
            if (myIntersections[5]->UVparameters[s].x() < myIntersections[5]->UVparameters[i3a.y()].x()) //>=?
            {
                tmpPos = patch->ComputePos(myIntersections[5]->UVparameters[s].z(), myIntersections[5]->UVparameters[s].w());
                du = QVector3D(patch->ComputeDu(myIntersections[5]->UVparameters[s].z(), myIntersections[5]->UVparameters[s].w()));
                dv = QVector3D(patch->ComputeDv(myIntersections[5]->UVparameters[s].z(), myIntersections[5]->UVparameters[s].w()));
                Norm = QVector3D::crossProduct(du, dv).normalized();
                Scale(additionalMat, Norm, tmpPos);
                result.append(tmpPos);
            }
        }

        //****WRITE****


        for (int i = 0; i<result.count(); i++)
            stream << "N" << count << "G01" << "X" << QString::number(result[i].z()-centering, 'f', 3) << "Y" << QString::number(result[i].y()-centering, 'f', 3) << "Z" << QString::number(groundLevel, 'f', 3) << endl;
         stream << "N" << count << "G01" << "X" << QString::number(result.last().z()-centering, 'f', 3) << "Y" << QString::number(result.last().y()-centering, 'f', 3) << "Z" << QString::number(maxZPos, 'f', 3) << endl;

    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
}

QVector<QVector4D> Path3C::GetPositions(QVector<QVector4D> inters, CADSplinePatch* patch, float additionalMat)
{
    QVector4D tmpPos;
    QVector3D du;
    QVector3D dv;
    QVector3D Norm;
    QVector<QVector4D> p;

    for (int i = 0; i< inters.count(); i++) {
        tmpPos = patch->ComputePos(inters[i].z(), inters[i].w());
        du = QVector3D(patch->ComputeDu(inters[i].z(), inters[i].w()));
        dv = QVector3D(patch->ComputeDv(inters[i].z(), inters[i].w()));
        Norm = QVector3D::crossProduct(du, dv).normalized();
        Scale(additionalMat, Norm, tmpPos);
        tmpPos.setX(tmpPos.x() + groundLevel - additionalMat);
        p.append(tmpPos);
    }
    return p;
}

QVector2D Path3C::FindIntersectionOfIntersections(QVector<QVector4D> inters1, QVector<QVector4D> inters2)
{
    //compare based on square patch intersecting all patches (patch1)
    /*float tmpDist = fabs(inters1[0].x() - inters2[0].x())
            + fabs(inters1[0].y() - inters2[0].y());
    QVector2D intersIndex = QVector2D(0, 0);

    for (int i = 0; i < inters1.count(); i++)
    {
        for (int j = 0; j < inters2.count(); j++)
        {
            newDist = fabs(inters1[i].x() - inters2[j].x()) + fabs(inters1[i].y() - inters2[j].y());
            if (newDist < tmpDist) {
                tmpDist = newDist;
                intersIndex.setX(i);
                intersIndex.setY(j);
            }
        }
    }
    return intersIndex;*/


    QVector2D intersIndex = QVector2D(0, 0);

    float newDist;
    float tmpDist = fabs(inters1[0].x() - inters2[0].x()) + fabs(inters1[0].y() - inters2[0].y()) + fabs(inters1[0].z() - inters2[0].z());
    for (int i = 0; i < inters1.count(); i++)
    {
        for (int j = 0; j < inters2.count(); j++)
        {
            newDist = fabs(inters1[i].x() - inters2[j].x()) + fabs(inters1[i].y() - inters2[j].y()) + fabs(inters1[i].z() - inters2[j].z());
            if (newDist < tmpDist) {
                tmpDist = newDist;
                intersIndex.setX(i);
                intersIndex.setY(j);
            }
        }
    }
    return intersIndex;
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

void Path3C::PatchSamplingRange(QVector<QVector4D> *result, CADSplinePatch *patch, float additionalMat, float startU, float endU, float startV, float endV)
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
            if(tmpPos.x() >= groundLevel ) result->append(tmpPos);
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

        int precision2 = 5;
        int precision1 = 4;
        int count = 3;
        bool isEven = 0;
        bool isEmpty = false;
        bool isPrevEmpty = false;
        bool isNextEmpty = false;
        //TODO: move cutter to start position

        for (int y = 0; y < 150; y+=precision1) {
            isEmpty = false;
            isPrevEmpty = false;

            for (int x = 0; x < 149; x+=precision2) {
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

double Path3C::GetAngle(QPointF pivot, QPointF source, QPointF dest)
{
    QPointF s = QPointF(source.x() - pivot.x(), source.y() - pivot.y());
    QPointF d = QPointF(dest.x() - pivot.x(), dest.y() - pivot.y());

    float dotProduct = s.x() * d.x() + s.y() * d.y();
    float crossProduct = s.x() * d.y() - s.y() * d.x();

    return atan2(crossProduct, dotProduct);
}

bool Path3C::isEnclosed(QVector<QVector4D> polygon, float u, float v) //z y
{
    double det = 0;
    double totalAngle = 0;
    float epsilon = 0.01f;
    // If subject it's on polygon edge, it's inside of polygon.
    for(int i = 0; i < polygon.count(); i++)
    {
        QVector4D from = polygon[i];
        QVector4D to = polygon[(i + 1) % polygon.count()];

        /*det = from.x() * to.y() + to.x() * v + u * from.y() - u * to.y() - from.x() * v - to.x() * from.y();
        if(det == 0)
        {
            if ((fmin(from.z(), to.z()) <= u) && (u <= fmax(from.z(), to.z())) &&
                (fmin(from.y(), to.y()) <= v) && (v <= fmax(from.y(), to.y())))
                return true;
        }*/
        totalAngle = GetAngle(QPointF(u,v), QPointF(polygon[polygon.count() - 1].z(), polygon[polygon.count() - 1].y()), QPointF(polygon[0].z(), polygon[0].y()));
        for (int i = 0; i < polygon.count() - 1; i++)
            totalAngle += GetAngle(QPointF(u,v), QPointF(polygon[i].z(), polygon[i].y()), QPointF(polygon[i + 1].z(), polygon[i + 1].y()));
    }


    bool condition = fabs(totalAngle) < epsilon;
    //side? condition = (fabs(totalAngle) < epsilon) : condition = (fabs(totalAngle) >= epsilon);

    if (condition)
        return false;
    else
        return true;
}

