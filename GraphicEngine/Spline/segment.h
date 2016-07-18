#ifndef SEGMENT_H
#define SEGMENT_H

#include <QList>
#include "marker.h"
//class Marker;

class Segment {
public:
    Segment();
    Segment(QList<Marker*> m);
    int pointsNo;
    QList<Marker*> markers;
};

#endif // SEGMENT_H
