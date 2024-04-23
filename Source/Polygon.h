#ifndef POLYGON
#define POLYGON

#include <raylib.h>
#include <vector>
using namespace std;

#define ZERO { 0, 0 }

// This class allows loading and generation of polygons and
// then converting them to appropriate data formats for the Gpu.
class Polygon
{
public:
    vector<Vector2> points;
    Polygon();
    void Generate(int n);
};

#endif