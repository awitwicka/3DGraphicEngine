#include "path3c.h"

#include <QFile>

Path3C::Path3C()
{

}

Path3C::Path3C(Widget *context)
{
    this->context = context;

    for (int i = 0; i<150; i++) {
        for (int j = 0; j<150; j++)
            heightMap[i][j] = groundLevel;
    }
}

void Path3C::GeneratePath()
{
    // y
    // |
    // |___ z
    //
    //1500*2 x 1500*2
    QVector4D tmpPos;
    for (int i = 0; i< context->SplinePatches.length(); i++) {

    for (float u = 0; u <= 1; u+=sampling) {
        for (float v = 0; v <= 1; v+=sampling) {
            tmpPos = context->SplinePatches[i]->ComputePos(u, v);
            tmpPos/=scaleFactor;
            tmpPos.setZ(tmpPos.z() + offset);
            tmpPos.setY(tmpPos.y() + offset);
            if (heightMap[(int)tmpPos.z()][(int)tmpPos.y()] < groundLevel+tmpPos.x() )
                heightMap[(int)tmpPos.z()][(int)tmpPos.y()] = groundLevel+tmpPos.x();
        }
    }

    }

    SavePath();
}

void Path3C::SavePath()
{
    QString path = "E:\\Path.txt";
    QFile file( path );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );

        int start, end, tmp;
        int count = 3;
        start = 150;
        end = 0;
        //TODO: move cutter to start position
        for (int y = 0; y < 150; y++) {
            //swap
            tmp = start;
            start = end;
            end = tmp;

            for (int x = start; x < end; x++) {
                stream << "N" << count << "G01" << "X" << QString::number(x-offset, 'f', 3) << "Y" << QString::number(y-offset, 'f', 3) << "Z" << QString::number(heightMap[x][y], 'f', 3) << endl;
                count++;
            }
        }
    } else {
        qWarning() << "Failed to open" << file.fileName() << "for write:" << file.errorString();
    }
}

