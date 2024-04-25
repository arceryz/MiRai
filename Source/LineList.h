#ifndef LINELIST_H
#define LINELIST_H

#include <raylib.h>
#include <vector>
using namespace std;

#define ZERO { 0, 0 }

// This class allows loading and generation of polygons and
// then converting them to appropriate data formats for the Gpu.
class LineList
{
public:
    vector<Vector2> lines;
    LineList();
    void GenerateRegularPolygon(int n);
};

#endif