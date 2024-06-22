#include "Ray2Scene.h"
#include <raymath.h>
#include <algorithm>

float GetRandomFloat(float from, float to) {
    return from + (to-from)*(float)GetRandomValue(0, INT_MAX) / INT_MAX;
}

Ray2Scene::Ray2Scene()
{

}
void Ray2Scene::Clear()
{
    mirrors.clear();
    numMirrors = 0;
    dirty = true;
}
void Ray2Scene::GenerateRegularPolygon(int n)
{
    Clear();
    
    // First generate the points. We always generate clockwise order.
    vector<Vector2> verts;
    verts.resize(n);
    for (int i = 0; i < n; i++) {
        float angle = PI/2 + 2*PI*(float)i/n;
        Vector2 point = { (float)cos(angle), (float)sin(angle) };
        verts[i] = point;
    }
    AddMirrorStrip(verts);
}
void Ray2Scene::GenerateRandomCirclePolygon(int n, float minSpacing)
{
    Clear();

    // Generate random angles.
    vector<float> angles;
    angles.resize(n);

    for (int tries = 0; tries < 100; tries++) {
        for (int i = 0; i < n; i++) angles[i] = GetRandomFloat(0, 360);
        sort(angles.begin(), angles.end());
        bool valid = true;
        for (int i = 0; i < n; i++) {
            float diff = abs(angles[(i+1)%n] - angles[i]);
            if (diff < minSpacing) {
                valid = false;
            }
        }
        if (valid) break;
    }

    // Create the points.
    vector<Vector2> points;
    for (int i = 0; i < n; i++) {
        float a = DEG2RAD * angles[i];
        Vector2 vec = { cos(a), sin(a) };
        points.push_back(vec);
    }
    AddMirrorStrip(points);
}
void Ray2Scene::AddMirrorStrip(vector<Vector2> &strip)
{
    // Make line segments.
    int n = strip.size();
    numMirrors += n;
    for (int i = 0; i < n; i++) {
        mirrors.push_back(strip[i]);
        mirrors.push_back(strip[(i+1)%n]);
    }
    dirty = true;
}