#ifndef SEGMENT_H
#define SEGMENT_H

#include "marker.h"
#include <QList>


class Segment {
public:
    Segment();
    Segment(QList<Marker*> m);
    int pointsNo;
    QList<Marker*> markers;
};

#endif // SEGMENT_H
