#include "Polygon.h"
#include <raymath.h>

Polygon::Polygon()
{

}

void Polygon::Generate(int n)
{
    points.clear();
    points.resize(n);

    // First generate the points. We always generate clockwise order.
    for (int i = 0; i < n; i++) {
        float angle = PI/2 + 2*PI*(float)i/n;
        Vector2 point = { (float)cos(angle), (float)sin(angle) };
        points[i] = point;
    }
}