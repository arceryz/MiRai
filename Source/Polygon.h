#ifndef POLYGON
#define POLYGON

#include <raylib.h>
#include <vector>
using namespace std;

#define ZERO { 0, 0 }

// The LineSegment struct is sent to the Gpu.
// It encodes the necessary information of a mirror plane,
// both curved and straight.
// This structure should be aligned according to std140 packing rules.
struct LineSegment {
    Vector2 u;
    Vector2 v;
};

// This class allows loading and generation of polygons and
// then converting them to appropriate data formats for the Gpu.
class Polygon
{
public:
    vector<LineSegment> segments;
    Polygon();
    void Generate(int n);
};

#endif