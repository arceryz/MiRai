#ifndef RAY3SCENE_H
#define RAY3SCENE_H

#include <raylib.h>
#include <vector>
using namespace std;

struct Face {
    vector<Vector3> vertices;
    Vector3 center;
    Vector3 normal;

    void ComputeCenter();
};

class Ray3Scene
{
    vector<Face> faces;
public:
    Ray3Scene();
    void AddModel(Model model);
    void AddMesh(Mesh mesh);
    void DebugDrawMesh(Mesh mesh);
    void DrawFaces(Color color, Color normalColor);
};

#endif