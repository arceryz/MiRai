#include "LineList.h"
#include <raymath.h>

LineList::LineList()
{

}

void LineList::GenerateRegularPolygon(int n)
{
    lines.clear();
    lines.resize(2*n);

    // First generate the points. We always generate clockwise order.
    vector<Vector2> verts;
    verts.resize(n);
    for (int i = 0; i < n; i++) {
        float angle = PI/2 + 2*PI*(float)i/n;
        Vector2 point = { (float)cos(angle), (float)sin(angle) };
        verts[i] = point;
    }

    // Make line segments.
    for (int i = 0; i < n; i++) {
        lines[2*i] = verts[i];
        lines[2*i+1] = verts[(i+1)%n];
    }
}