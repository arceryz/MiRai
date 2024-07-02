#ifndef RAY2SCENE_H
#define RAY2SCENE_H

#include <raylib.h>
#include <vector>
using namespace std;

#define ZERO { 0, 0 }

// This class allows loading and generation of polygons and
// then converting them to appropriate data formats for the Gpu.
class Ray2Scene
{
private:
public:
    vector<Vector2> mirrors;
    int numMirrors;
    bool dirty = false;
    Ray2Scene();
    void GenerateRegularPolygon(int n);
    void GenerateRandomCirclePolygon(int n, float minSpacing=0);
    void GenerateTriangle(float alpha, float beta, float gamma);
    void GenerateNiceRhombus();
    void GenerateTokarsky();
    void Center();
    void Clear();
    void AddMirrorStrip(vector<Vector2> &strip);
};

#endif