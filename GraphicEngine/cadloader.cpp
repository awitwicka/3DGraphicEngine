#include "cadloader.h"
#include <Qfile>
#include <QCoreApplication>
#include <Spline/intersection.h>

CADLoader::CADLoader()
{

}

CADLoader::CADLoader(Widget *context)
{
    this->context = context;
}

CADLoader::~CADLoader()
{

}

void CADLoader::LoadFile()
{   
    QString path = "E:\\Data.txt";
    QFile file( path );
    if ( file.open(QIODevice::ReadOnly) )
    {
        QTextStream stream( &file );
        /*****MARKERS******/
        QString line = stream.readLine();
        int n = line.toInt();
        for (int i = 0; i<n; i++) {
            line = stream.readLine();
            line.replace(",", ".");
            //for (QStringList )
            QStringList l = line.trimmed().split(" ");
            QString x = l[0];
            QString y = l[1];
            QString z = l[2];
            context->markers.append(Marker(x.toFloat()*100, y.toFloat()*100, z.toFloat()*100));
        }


        /*****OBJECTS*****/
        int objNo = stream.readLine().toInt();
        for (int i = 0; i<objNo; i++) {
            line = stream.readLine();
            QStringList l = line.split(" ");
            QList<Marker*> ref;
            //TODO: object name

            /******CURVES*******/
            if (l[0] == "BEZIERCURVE" ||l[0] == "BSPLINECURVE" || l[0] == "INTERP") {
                ref.clear();
                int no = stream.readLine().toInt();
                QStringList points = stream.readLine().split(" ");
                for (int k = 0; k<no; k++) {
                    QString a = points[k];
                    ref.append(&context->markers[a.toInt()]);
                }
                line = stream.readLine(); //END
                if (l[0] == "BEZIERCURVE") {
                    CADMarkerObject* b = new Bezier(ref, context->worldMatrix);
                    context->Splines.append(b);
                } else if (l[0] == "BSPLINECURVE") {
                    CADMarkerObject* b = new CurveC2(ref, context->worldMatrix);
                    context->Splines.append(b);
                } else if (l[0] == "INTERP") {
                    CADMarkerObject* b = new BSInterpolation(ref, context->worldMatrix);
                    context->Splines.append(b);
                }
            } else if (l[0] == "INTERS") {
                QVector<QVector4D> UVparams;
                CADSplinePatch *patch1;
                CADSplinePatch *patch2;

                QStringList params = stream.readLine().split(" ");
                int n = params[0].toInt();
                for (auto &s : context->SplinePatches) {
                    if (s->name == params[1])
                        patch1 = s;
                    if (s->name == params[2])
                        patch2 = s;
                }
                for (int k = 0; k < n; k++) {
                    QStringList p = stream.readLine().split(" ");
                    UVparams.push_back(QVector4D(p[0].toFloat(), p[1].toFloat(), p[2].toFloat(), p[3].toFloat()));
                }
                line = stream.readLine(); //END

                CADMarkerObject* b = new Intersection(context->worldMatrix, patch1, patch2, UVparams);
                context->Splines.append(b);

            /******SURFACES*******/
            } else if (l[0] == "BEZIERSURF" || l[0] == "BSPLINESURF") {
                ref.clear();
                QStringList params = stream.readLine().split(" ");
                int nMark = params[1].toInt(); //N - oś X
                int mMark = params[0].toInt(); //M - oś Z
                QString type = params[2];  //X = {C, R} Cylinder/Rectangle
                QString joinType = params[3];  //Y = {H, V} Horizontal/Vertical

                QStringList points = stream.readLine().split(" ");
                /*QList<Marker*> markers;
                for (int k = 0; k<nMark*mMark; k++) {
                    QString a = points[k];
                    markers.append(&context->markers[a.toInt()]);
                }*/
                //rearder markers
                QList<Marker*> reordered;
                for(int k = 0; k<mMark; k++) { //y axis
                    for(int l = 0; l<nMark; l++) { //z axis
                        int index = (mMark)*l+k;
                        QString a = points[index];
                        reordered.append(&context->markers[a.toInt()]);
                        //reordered.append(markers[index]);
                    }
                }

                line = stream.readLine();
                QString a;
                if (l[0] == "BEZIERSURF") {
                    bool isPlane = (type.at(0) == 'R')? true : false;
                    bool isHoriz = (joinType.at(0) == 'H')? true : false;

                    int xm;
                    int ym;
                    if (isPlane) {
                        xm = (nMark-4)/3+1;
                        ym = (mMark-4)/3+1;
                    } else {
                        xm = (nMark-4+1)/3+1;
                        ym = (mMark-4)/3+1;
                    }
                    CADSplinePatch* b = new BezierPlane(context->worldMatrix, reordered, xm, ym, isHoriz, isPlane);
                    context->SplinePatches.append(b);
                } else if (l[0] == "BSPLINESURF") {
                    bool isPlane = (type.at(0) == 'R')? true : false;
                    bool isHoriz = (joinType.at(0) == 'H')? true : false;

                    /*if (isPlane == false) {
                        markers.clear();
                        for (int k = 0; k<mMark; k++) {
                            for (int l=0; l<nMark; l++) {
                                a = points[l+(k*nMark)];
                                markers.append(&context->markers[a.toInt()]);
                            }
                            a = points[(k*nMark)];
                            markers.append(&context->markers[a.toInt()]);
                            markers.append(&context->markers[a.toInt()+1]);
                            markers.append(&context->markers[a.toInt()+2]);
                        }
                    }*/

                    int xm;
                    int ym;
                    if (isPlane == true) {
                        xm = nMark+1-4;
                        ym = mMark+1-4;
                    } else {
                        xm = nMark+1-4+3;
                        ym = mMark+1-4;
                    }
                    CADSplinePatch* b = new BSplinePlane(context->worldMatrix, reordered, xm, ym, isHoriz, isPlane);
                    context->SplinePatches.append(b);
                }
            }
        }

    }
    context->UpdateSceneElements();
    context->update();
}

void CADLoader::SaveFile()
{
    //QString path = QCoreApplication::applicationFilePath();
    //        path.append("/Data.txt");
    QString path = "E:\\Data.txt";
    QFile file( path );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        //#REFERENCE POINTS
        QTextStream stream( &file );
        //QList<Marker> m;
        int n = context->markers.length();
        stream << n << endl;

        for (int i = 0; i<context->markers.length(); i++) {
            stream << context->markers[i].point.x()/100 << " " << context->markers[i].point.y()/100 << " " << context->markers[i].point.z()/100 << endl;
            //m.append(context->markers[i]);
        }
        //#OBJECTS
        n =context->Splines.length();
        n+=context->SplinePatches.length();
        stream << n << endl;

        //SURFACES
        for (int i = 0; i< context->SplinePatches.length(); i++) {
            CADSplinePatch* patch = context->SplinePatches[i];
            QString type = patch->idname.at(0);
            char t = (patch->isPlane)? 'R' : 'C';
            char o = (patch->isHorizontal)? 'H' : 'V';

            if(type.at(0) == 'g')  //bezier surf
                stream << "BEZIERSURF" << " " << patch->name << endl;
            else if (type.at(0) == 'k')  //bspline surf
                stream << "BSPLINESURF" << " " << patch->name << endl;

            stream << patch->MarkerM << " " << patch->MarkerN << " " << t << " " << o << endl;
            //for(int k = 0; k<context->SplinePatches[i]->markers.length(); k++)
            //    stream << context->markers.indexOf(*context->SplinePatches[i]->markers[k]) << " ";

            //reverse order to suit common format
            for(int k = 0; k<patch->MarkerN; k++) { //y axis
                for(int l = 0; l<patch->MarkerM; l++) { //z axis
                    int index = (patch->MarkerN)*l+k;
                    stream << context->markers.indexOf(*patch->markers[index]) << " ";
                }
            }

            stream << endl << "END" << endl;
        }
        //SPLINES
        for (int i = 0; i< context->Splines.length(); i++) {
            QString type = context->Splines[i]->idname.at(0);
            if(type.at(0) == 'b') { //bezier
                stream << "BEZIERCURVE" << " " << context->Splines[i]->name << endl;
                stream << context->Splines[i]->markers.length() << endl;
                for(int k = 0; k<context->Splines[i]->markers.length(); k++) {
                    stream << context->markers.indexOf(*context->Splines[i]->markers[k]) << " ";
                }
                stream << endl << "END" << endl;
            } else if(type.at(0) == 'c') { //bspline
                stream << "BSPLINECURVE" << " " << context->Splines[i]->name << endl;
                stream << context->Splines[i]->markers.length() << endl;
                for(int k = 0; k<context->Splines[i]->markers.length(); k++) {
                    stream << context->markers.indexOf(*context->Splines[i]->markers[k]) << " ";
                }
                stream << endl << "END" << endl;
            } else if (type.at(0) == 's') { //interpolation
                stream << "INTERP" << " " << context->Splines[i]->name << endl;
                stream << context->Splines[i]->markers.length() << endl;
                for(int k = 0; k<context->Splines[i]->markers.length(); k++) {
                    stream << context->markers.indexOf(*context->Splines[i]->markers[k]) << " ";
                }
                stream << endl << "END" << endl;
            } else if (type.at(0) == 'x') { //intersection
                stream << "INTERS" << " " << context->Splines[i]->name << endl;
                Intersection* inter = dynamic_cast<Intersection*>(context->Splines[i]);
                stream << inter->UVparameters.length() << " " << inter->patch1->name << " " << inter->patch2->name << endl;
                for(int k = 0; k<inter->UVparameters.length(); k++) {
                    stream << inter->UVparameters[k].x() << " " << inter->UVparameters[k].y() << " " << inter->UVparameters[k].z() << " " << inter->UVparameters[k].w() << endl;
                }
                stream << "END" << endl;
            }
        }

    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
}


