#ifndef RAY3SCENE_H
#define RAY3SCENE_H

#include <raylib.h>
#include <vector>
#include "TriangleNet.h"
using namespace std;

class Ray3Scene
{
private:
    vector<Polygon> mirrors;
public:
    Ray3Scene();
    void AddMirrorModel(Model model, bool flipNormals);
    void AddMirrorMesh(Mesh mesh, bool flipNormals);
    void DebugDrawMesh(Mesh mesh);
    void DrawMirrors(vector<Color> colors, float faceScale=1.0f);
};

#endif