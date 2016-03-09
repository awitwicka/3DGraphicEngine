#include "elipse.h"

Elipse::Elipse(): a(50), b(100), c(50)
{
    InitializeElipse();
}

void Elipse::InitializeElipse()
{
    D = QMatrix4x4(1/(a*a),0,0,0,
                   0,1/(b*b),0,0,
                   0,0,1/(c*c),0,
                   0,0,0,-1); //odwrotnosci kwadratow a = a/50^2;
}

void Elipse::doWork(/*float widgetWidth, float widgetHeight, QMatrix matrix*/)
{
    qWarning() << "Hello I'm a new Thread!";

    QImage image(widgetWidth,widgetHeight,QImage::Format_RGB32);
    QPainter painter(&image);
    //painter.begin(&image);
    painter.setViewport(image.width()/2,image.height()/2,image.width(),image.height());
    painter.setPen(Qt::white);
    painter.drawRect(-10,-10,50,50);
    //painter.end();
    //QPainter painter(&image);
    //painter.fillRect(image.rect(),Qt::black);


    //light params
    QVector4D n;
    QVector4D v = QVector4D(0,0,1,1);
    float I;
    float m = 50.0; //TODO: set as global
    QColor color;
    //widget dimentions
    QPoint topLeft = QPoint(-widgetWidth/2,-widgetHeight/2);
    QPoint botRight = QPoint(1.5f*widgetWidth,1.5f*widgetHeight);//painter.viewport().bottomRight();

    for (int y = topLeft.y(); y < botRight.y(); y++) {
        for (int x = topLeft.x(); x < botRight.x(); x++) {
            float z = f(x, y, matrix);
            if (z != -1) {
               //Intensity of the light
               n = fd(x, y, z, matrix);
               n.normalize();
               float dot = QVector4D::dotProduct(v, n);
               I = pow(dot,m); //TODO: add m
               qWarning() << "intensity:" << dot;
               //color.setRgb(I*100,I*100,0);
               color.setHsl(60, 100, I*100);
               painter.setPen(color);
               //painter.setPen(Qt::yellow);
               painter.drawEllipse(QPoint(x, y), 1, 1);
            }
        }
    }
    //image = d->input.scaled( d->size, d->aspectMode, Qt::SmoothTransformation );
    painter.end();
    emit workFinished(image);
}

float Elipse::f(float x, float y, QMatrix4x4 m)
{
    QMatrix4x4 Dm = m.inverted().transposed()*D*m.inverted();

    //(a11x^2 + a22y^2 + 2a12xy + 2a14x + 2a24y + a44)+
    //+2(a13x + a23y + a34)z + a33z^2 = 0
    QVector4D row1 = Dm.row(0);
    QVector4D row2 = Dm.row(1);
    QVector4D row3 = Dm.row(2);
    QVector4D row4 = Dm.row(3);
    float ax = row3.z();
    float bx = 2*(row1.z()*x + row2.z()*y + row3.w());
    float cx = row1.x()*x*x + row2.y()*y*y + 2*row1.y()*x*y + 2*row1.w()*x + 2*row2.w()*y + row4.w();
    float delta = bx*bx - (4*ax*cx);
    if (delta<0)
        return -1;
    float z1 = (-bx + sqrt(delta)) / (2*ax);
    float z2 = (-bx - sqrt(delta)) / (2*ax);
    if (z1 >= z2)
        return z1;
    else
        return z2;

    /*
    float zz = -((x*x)/(a*a) + (y*y)/(b*b) -1)*(c*c);
    if (zz < 0)
        return -1;
    else
        return zz;//return sqrt(zz);
    */
}

QVector4D Elipse::fd(float x, float y, float z, QMatrix4x4 m)
{
    QMatrix4x4 Dm = m.inverted().transposed()*D*m.inverted();
    QVector4D row1 = Dm.row(0);
    QVector4D row2 = Dm.row(1);
    QVector4D row3 = Dm.row(2);
    QVector4D row4 = Dm.row(3);

   return 2*QVector4D(x*row1.x()+y*row1.y()+z*row1.z()+row1.w(),
                    x*row2.x()+y*row2.y()+z*row2.z()+row2.w(),
                    x*row3.x()+y*row3.y()+z*row3.z()+row3.w(),
                    x*row4.x()+y*row4.y()+z*row4.z()+row4.w());
}

void Elipse::setWidgetHeight(float value)
{
    widgetHeight = value;
}

void Elipse::setWidgetWidth(float value)
{
    widgetWidth = value;
}

void Elipse::setA(float value)
{
    a = value;
}

void Elipse::setB(float value)
{
    b = value;
}

void Elipse::setC(float value)
{
    c = value;
}

void Elipse::setM(const QMatrix4x4 &value)
{
    matrix = value;
}
