#ifndef RAY3_PROGRAM_H
#define RAY3_PROGRAM_H

#include <raylib.h>
#include "Ray3Scene.h"

// This class represents the 3D mirror raytracer.
// It does not use compute shaders and is implemented purely with
// fragment shaders.
// In the future we can store distances and render them as points in 3D instead,
// It would be cool to visualise the edges in this way.
class Ray3Program
{
public:
    int numBounces = 0;
    Color color = GREEN;

    float arcFocus = 1000.0f;

    Ray3Program(Ray3Scene &_scene);
    void UpdatePoints(int num, Vector2 *points);
    void RenderPass();

private:
    Ray3Scene &scene;
    Shader renderProgram = {};
    int numPoints = 0;
};

#endif