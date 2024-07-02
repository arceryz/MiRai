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

void Ray2Scene::Center()
{
    Vector2 sum = {};
    for (Vector2 vec: mirrors) {
        sum = Vector2Add(sum, vec);
    }
    sum = Vector2Scale(sum, 1.0f/mirrors.size());
    for (int i = 0; i < mirrors.size(); i++) {
        mirrors[i] = Vector2Subtract(mirrors[i], sum);
    }
}

void Ray2Scene::GenerateTriangle(float alpha, float beta, float gamma)
{
    float sina = sinf(alpha);
    float sinb = sinf(beta);
    float sinc = sinf(gamma);

    float A = 1;
    float B = sinb / sina;
    float C = sinc / sina;

    Vector2 c = { (A + C*cos(beta)) / 3.0, C*sin(beta) / 3.0 };
    
    Vector2 v1 = { 0, 0 };
    Vector2 v2 = { A, 0 };
    Vector2 v3 = { C*cos(beta), C*sin(beta) };

    vector<Vector2> verts = { v1, v2, v3 };
    Clear();
    AddMirrorStrip(verts);
    Center();
}

void Ray2Scene::GenerateNiceRhombus()
{
    float ang = 2.0*PI/3.0;
    Vector2 v1 = { 0, 0 };
    Vector2 v2 = { cosf(ang), sinf(ang) };
    Vector2 v3 = { v2.x + 1.0, v2.y };
    Vector2 v4 = { 1.0, 0.0 };
    vector<Vector2> verts = { v1, v2, v3, v4 };
    Clear();
    AddMirrorStrip(verts);
    Center();
}

void Ray2Scene::GenerateTokarsky()
{
    vector<Vector2> verts = {
        { -1, 0 },
        { 1, 0 },
        { 1, 1 },
        { 2, 2 },
        { 2, 1 },
        { 3, 1 },
        { 3, 0 },
        { 4, 0 },
        { 3, -1 },
        { 2, -1 },
        { 2, -2 },
        { 1, -1 },
        { 0, -1 },
        { 0, -2 },
        { -1, -1 },
        { -2, -1 },
        { -2, -2 },
        { -3, -1 },
        { -3, 0 },
        { -4, 0 },
        { -3, 1 },
        { -2, 1 },
        { -2, 2 },
        { -1, 1 },
        { -1, 0 }
    };
    reverse(verts.begin(), verts.end());
    Clear();
    AddMirrorStrip(verts);
    Center();
}