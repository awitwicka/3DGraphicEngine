#include "cadloader.h"
#include <Qfile>
#include <QCoreApplication>

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
        QList<Marker> m;

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
            m.append(Marker(x.toFloat()*100, y.toFloat()*100, z.toFloat()*100));
        }
        int objNo = stream.readLine().toInt();
        for (int i = 0; i<objNo; i++) {
            line = stream.readLine();
            QStringList l = line.split(" ");
            QList<Marker*> ref;
            if (l[0] == "BEZIERCURVE" ||l[0] == "BSPLINECURVE" || l[0] == "INTERP") {
                ref.clear();
                int no = stream.readLine().toInt();
                QStringList points = stream.readLine().split(" ");
                for (int k = 0; k<no; k++) {
                    QString a = points[k];
                    int count = 0;
                    int index = 0;
                    for (int p = 0; p<context->markers.length(); p++) {
                        if (context->markers[p].idname == m[a.toInt()].idname) {
                            count++;
                            index = p;
                        }
                    }
                    if (count == 0) {
                        context->markers.append(m[a.toInt()]);
                        ref.append(&context->markers.last());
                    }else
                        ref.append(&context->markers[index]);
                }
                line = stream.readLine();
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
                /******SURFACES*******/
            } else if (l[0] == "BEZIERSURF" || l[0] == "BSPLINESURF") {
                ref.clear();
                QStringList params = stream.readLine().split(" ");
                int nMark = params[0].toInt();
                int mMark = params[1].toInt();
                QString type = params[2];
                QString joinType = params[3];

                QStringList points = stream.readLine().split(" ");
                QList<Marker> markers;
                for (int k = 0; k<nMark*mMark; k++) {
                    QString a = points[k];
                    markers.append(m[a.toInt()]);
                }
                line = stream.readLine();
                if (l[0] == "BEZIERSURF") {
                    bool isPlane = (type.at(0) == 'R')? true : false;
                    int xm;
                    int ym;
                    if (isPlane) {
                        xm = (nMark-4)/3+1;
                        ym = (mMark-4)/3+1;
                    } else {
                        xm = (nMark-4)/3+1+1;
                        ym = (mMark-4)/3+1;
                    }
                    CADSplinePatch* b = new BezierPlane(context->worldMatrix, 4, 4, xm, ym, 100, 100, 0, 0, 0, isPlane);
                    for (int r = 0; r<b->markers.length(); r++)
                        b->markers[r].point = markers[r].point;
                    context->SplinePatches.append(b);
                } else if (l[0] == "BSPLINESURF") {
                    bool isPlane = (type.at(0) == 'R')? true : false;
                    int xm;
                    int ym;
                    if (isPlane) {
                        xm = (nMark-4)+1;
                        ym = (mMark-4)+1;
                    } else {
                        xm = (nMark-4)+1+3;
                        ym = (mMark-4)+1;
                    }

                   /* QList<Marker> markersReorder;
                    int count = 0;
                    while (count<markers.length()) {
                        for(int r = 0; r<4; r++){
                        markersReorder.append(markers[count]);

                    }

                    for(int r = 0; r<; r++){
                        markersReorder.append(markers[4]);
                    for(int r = 0; r<nMark-1; r++){
                        markersReorder.append(markers[]);
                    }
                    */
                    CADSplinePatch* b = new BSplinePlane(context->worldMatrix, 4, 4, xm, ym, 100, 100, 0, 0, 0, isPlane);
                    for (int r = 0; r<b->markers.length(); r++)
                        b->markers[r].point = markers[r].point;
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
        QList<Marker> m;
        int n = context->markers.length();
        for (int i = 0; i<context->SplinePatches.length(); i++)
            n+=context->SplinePatches[i]->markers.length();
        stream << n << endl;

        for (int i = 0; i<context->markers.length(); i++) {
            stream << context->markers[i].point.x()/100 << " " << context->markers[i].point.y()/100 << " " << context->markers[i].point.z()/100 << endl;
            m.append(context->markers[i]);
        }
        for (int i = 0; i<context->SplinePatches.length(); i++) {
            for (int j = 0; j<context->SplinePatches[i]->markers.length(); j++) {
                stream << context->SplinePatches[i]->markers[j].point.x()/100 << " " << context->SplinePatches[i]->markers[j].point.y()/100 << " " << context->SplinePatches[i]->markers[j].point.z()/100 << endl;
                m.append(context->SplinePatches[i]->markers[j]);
            }
        }
        //#OBJECTS
        n =context->Splines.length();
        n+=context->SplinePatches.length();
        stream << n << endl;
        //SPLINES
        for (int i = 0; i< context->Splines.length(); i++) {
            QString type = context->Splines[i]->idname.at(0);
            if(type.at(0) == 'b') { //bezier
                stream << "BEZIERCURVE" << " " << context->Splines[i]->name << endl;
                stream << context->Splines[i]->markers.length() << endl;
                for(int k = 0; k<context->Splines[i]->markers.length(); k++) {
                    for (int l=0; l<m.length(); l++) {
                        if(m[l].idname==context->Splines[i]->markers[k]->idname)
                        stream << l << " ";
                    }
                }
                stream << endl << "END" << endl;
            } else if(type.at(0) == 'c') { //bspline
                stream << "BSPLINECURVE" << " " << context->Splines[i]->name << endl;
                stream << context->Splines[i]->markers.length() << endl;
                for(int k = 0; k<context->Splines[i]->markers.length(); k++) {
                    for (int l=0; l<m.length(); l++) {
                        if(m[l].idname==context->Splines[i]->markers[k]->idname)
                        stream << l << " ";
                    }
                }
                stream << endl << "END" << endl;
            } else if (type.at(0) == 's') { //interpolation
                stream << "INTERP" << " " << context->Splines[i]->name << endl;
                stream << context->Splines[i]->markers.length() << endl;
                for(int k = 0; k<context->Splines[i]->markers.length(); k++) {
                    for (int l=0; l<m.length(); l++) {
                        if(m[l].idname==context->Splines[i]->markers[k]->idname)
                        stream << l << " ";
                    }
                }
                stream << endl << "END" << endl;
            }
        }
        //SURFACES
        for (int i = 0; i< context->SplinePatches.length(); i++) {
            QString type = context->SplinePatches[i]->idname.at(0);
            if(type.at(0) == 'g') { //bezier surf
                stream << "BEZIERSURF" << " " << context->SplinePatches[i]->name << endl;
                char t = (context->SplinePatches[i]->isPlane)? 'R' : 'C';
                stream << context->SplinePatches[i]->MarkerN << " " << context->SplinePatches[i]->MarkerM << " " << t << " " << "H" << endl;
                for(int k = 0; k<context->SplinePatches[i]->markers.length(); k++) {
                    for (int l=0; l<m.length(); l++) {
                        if(m[l].idname==context->SplinePatches[i]->markers[k].idname)
                        stream << l << " ";
                    }
                }
                stream << endl << "END" << endl;
            } else if (type.at(0) == 'k') { //bspline surf
                stream << "BSPLINESURF" << " " << context->SplinePatches[i]->name << endl;
                char t = (context->SplinePatches[i]->isPlane)? 'R' : 'C';
                stream << context->SplinePatches[i]->MarkerN << " " << context->SplinePatches[i]->MarkerM << " " << t << " " << "H" << endl;
                for(int k = 0; k<context->SplinePatches[i]->markers.length(); k++) {
                    for (int l=0; l<m.length(); l++) {
                        if(m[l].idname==context->SplinePatches[i]->markers[k].idname)
                        stream << l << " ";
                    }
                }
                stream << endl << "END" << endl;
            }
        }

    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
}


