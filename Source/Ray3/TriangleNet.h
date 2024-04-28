#ifndef TRIANGLE_NET_H
#define TRIANGLE_NET_H

#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class Polygon {
public:
    vector<Vector3> vertices;
    Vector3 center = {};
    Vector3 normal = {};

    Polygon();
    void ComputeCenter();
    void Draw(Color color, Color normalColor, float faceScale, int numEdges=-1);
};

// A triangle net class is used visualise the triangle merging problem
// and to turn triangles into polygons (list of vertices).
class TriangleNet
{
public:
    TriangleNet();
    void Clear();
    vector<Polygon> GetPolygons();
    void AddTriangles(vector<Vector3> triangles);
    void Draw(Color internal, Color external);
    void DrawLabels(Camera3D camera, float size, Color color);

private:
    vector<Vector3> vertices;
    vector<int> indices;
    unordered_map<string, int> vertexToIndex;
    unordered_map<int, unordered_map<int, int>> indexToNeighbors;

    bool IsEdgeInternal(int u, int v);
    bool IsVertexInternal(int u);
    string GetVectorKey(Vector3 vec);
};

#endif