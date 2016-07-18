#include "segment.h"

Segment::Segment()
{

}

Segment::Segment(QList<Marker*> m)
{
    markers = m;
    pointsNo = m.length();
}
