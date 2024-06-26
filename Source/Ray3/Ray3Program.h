#ifndef RAY3_PROGRAM_H
#define RAY3_PROGRAM_H

#include <raylib.h>
#include "Ray3Scene.h"
#include "main.h"

typedef int ShaderBuffer;

// This class represents the 3D mirror raytracer.
// It does not use compute shaders and is implemented purely with
// fragment shaders.
// In the future we can store distances and render them as points in 3D instead,
// It would be cool to visualise the edges in this way.
class Ray3Program
{
public:
    int numMirrors = 0;
    int numBounces = 5;
    int lowerBounceLimit = 0;
    Color color = GREEN;
    float sphereFocus = FOCUS_INF;
    float edgeThickness = 1.0f;
    float markSize = 1.0f;
    float resolutionPercent = 0.5f;
    float falloff = 0.15f;
    bool dynamicResolution = true;
    bool showMark = false;
    bool showEdgeMark = false;
    bool showEdges = true;
    Color innerClearColor = { 0, 0, 0, 0 };
    Color edgeColor = { 0, 255, 0, 255 };

    Ray3Program();
    void SetScene(Ray3Scene *_scene);
    Ray3Scene* GetScene();
    void UpdatePoints(int num, Vector2 *points);
    void Draw();
    void Render(Camera3D camera);

private:
    Shader shader;
    ShaderBuffer mirrorVertexBuffer;
    ShaderBuffer mirrorBuffer;
    RenderTexture2D renderTexture;
    Ray3Scene *scene;
    Shader renderProgram = {};
    int resolution = 800;
};

#endif