#ifndef RAY2_PROGRAM_H
#define RAY2_PROGRAM_H

#include <raylib.h>

#define WORKGROUP_SIZE 1024
#define MAX_POINTS 1000
#define MAX_BOUNCES 16
#define MAX_WORKGROUPS 500
#define MAX_RAYS (MAX_WORKGROUPS*WORKGROUP_SIZE)
#define ARC_RADIUS_INF 1000

// This class represents the 2D mirror raytracer.
// It handles the compute and rendering part internally
// and exposes control parameters to the outside.
class Ray2Program
{
public:
    int numRays = 0;
    int numBounces = 0;
    Color color = GREEN;
    float falloff = 0.7;
    float pointSize = 1.0;

    // If the radius is +-INF, then we have the straight edges.
    // If the radius is positive, we enter spherical space.
    // If the radius is negative, we enter hyperbolic space.
    float arcRadius = ARC_RADIUS_INF;

    Ray2Program();
    void UpdatePoints(int num, Vector2 *points);
    void ComputePass();
    void RenderPass();

private:
    int pointSSBO = 0;
    int distanceSSBO = 0;
    int pointVAO = 0;
    int computeProgram = 0;
    Shader renderProgram = {};
    int numPoints = 0;
    int numWorkGroups = 0;
    float focus = 0;

    void ComputeFocus();
};

#endif