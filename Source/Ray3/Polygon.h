#ifndef POLYGON_H
#define POLYGON_H

#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class Polygon {
public:
    vector<Vector3> vertices;
    Vector3 normal = {};

    static string VectorKey(Vector3 vec);
    static vector<Polygon> FromTriangles(vector<Vector3> triangles);
    Polygon();
    Vector3 GetCenter();
    void Scale(float factor);
    void Translate(Vector3 vec);
    void Draw(Color color, Color normalColor, float scale, float faceScale, int numEdges=-1);
};

#endif