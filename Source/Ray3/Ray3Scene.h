#ifndef RAY3SCENE_H
#define RAY3SCENE_H

#include <raylib.h>
#include <vector>
#include "Polygon.h"
using namespace std;

// Face describes metadata of face.
// Must be tightly pack so we use pack(1) for 1-byte packing.
#pragma pack(1)
struct MirrorInfo {
    Vector4 normal;
    Vector4 center;
    int vertexOffset;
    int vertexCount;
    float p1, p2;
};

class Ray3Scene
{
public:
    vector<Polygon> mirrors;
    string name = "Unnamed Scene";
    Ray3Scene();
    void AddMirrorModel(Model model, bool flipNormals);
    void AddMirrorMesh(Mesh mesh, bool flipNormals);
    void DebugDrawMesh(Mesh mesh);
    void DrawMirrors(float scale=1.0f, float faceScale=1.0f);
    void NormalizeRadius();
    void Centralize();

    // Packing functions.
    vector<Vector4> GetMirrorVerticesPacked();
    vector<MirrorInfo> GetMirrorInfosPacked();
    int GetMirrorCount();
};

#endif