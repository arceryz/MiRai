#include "Polygon.h"
#include <raymath.h>

Polygon::Polygon()
{

}

void Polygon::Generate(int n)
{
    segments.clear();
    segments.resize(n);

    vector<Vector2> points;
    points.resize(n);

    // First generate the points. We always generate clockwise order.
    for (int i = 0; i < n; i++) {
        float angle = PI/2 + 2*PI*(float)i/n;
        Vector2 point = { (float)cos(angle), (float)sin(angle) };
        points[i] = point;
    }

    // Then set the points in the line segments.
    for (int i = 0; i < n; i++) {
        LineSegment seg;
        seg.u =  points[i];
        seg.v = points[(i+1)%n];
        segments[i] = seg;
    }
}