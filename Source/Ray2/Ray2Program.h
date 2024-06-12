#ifndef RAY2_PROGRAM_H
#define RAY2_PROGRAM_H

#include <raylib.h>
#include "Ray2Scene.h"
#include "main.h"

#define WORKGROUP_SIZE 1024
#define MAX_MIRRORS 1000
#define MAX_BOUNCES 16
#define MAX_WORKGROUPS 500
#define MAX_RAYS (MAX_WORKGROUPS*WORKGROUP_SIZE)

typedef int ShaderBuffer;
typedef int ComputeShader;
typedef int VertexArray;

// This class represents the 2D mirror raytracer.
// It handles the compute and rendering part internally
// and exposes control parameters to the outside.
class Ray2Program
{
public:
    int numRays = 0;
    int numBounces = 0;
    Color color = GREEN;
    float falloff = 0.3f;
    float pointSize = 1.0;
    float zoom = 1.0f;
    Vector2 origin = { 0, 0 };

    // If the focus is +-INF, then we have the straight edges.
    // If the focus is positive, we enter spherical space.
    // If the focus is negative, we enter hyperbolic space.
    float arcFocus = FOCUS_INF;

    Ray2Program(Ray2Scene &_scene);
    // Update the mirrors as a sequence of line segments.
    // Does not need to connect!
    void UpdateScene(bool center=true);
    void ComputePass();
    void RenderPass();
    void InterfacePass();
    float GetArcRadius();
    Vector2 Transform(Vector2 local);

private:
    ShaderBuffer mirrorSSBO = 0;
    ShaderBuffer distanceSSBO = 0;
    VertexArray pointVAO = 0;
    ComputeShader computeProgram = 0;
    Shader renderProgram = {};
    int numWorkGroups = 0;
    Ray2Scene &scene;
};

#endif