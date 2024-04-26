#include "LineList.h"
#include <raymath.h>
#include <algorithm>

float GetRandomFloat(float from, float to) {
    return from + (to-from)*(float)GetRandomValue(0, INT_MAX) / INT_MAX;
}

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
    LoadLineStrip(verts);
}
void LineList::GenerateRandomCirclePolygon(int n, float minSpacing)
{
    // Generate random angles.
    vector<float> angles;
    angles.resize(n);

    for (int tries = 0; tries < 100; tries++) {
        for (int i = 0; i < n; i++) angles[i] = GetRandomFloat(0, 360);
        sort(angles.begin(), angles.end());
        bool valid = true;
        for (int i = 0; i < n; i++) {
            if (abs(angles[(i+1)%n] - angles[i]) < minSpacing) {
                valid = false;
                break;
            }
        }
        if (valid) break;
    }

    // Create the points.
    vector<Vector2> points;
    for (int i = 0; i < n; i++) {
        printf("%f\n", angles[i]);
        float a = DEG2RAD * angles[i];
        Vector2 vec = { cos(a), sin(a) };
        points.push_back(vec);
    }
    LoadLineStrip(points);
}
void LineList::LoadLineStrip(vector<Vector2> &strip)
{
    int n = strip.size();
    lines.clear();
    lines.resize(2*n);

    // Make line segments.
    for (int i = 0; i < strip.size(); i++) {
        lines[2*i] = strip[i];
        lines[2*i+1] = strip[(i+1)%n];
    }
}