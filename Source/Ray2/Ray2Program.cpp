#include <stdlib.h>
#include <algorithm>
#include "Ray2Program.h"
#include <raymath.h>
#include <rlgl.h>
using namespace std;

Ray2Program::Ray2Program(Ray2Scene &_scene)
: scene(_scene)
{
    renderProgram = LoadShader("Shaders/ray2.vert", "Shaders/ray2.frag");
    
    // Compute shader.
    char *code = LoadFileText("Shaders/ray2.comp");
    int shader = rlCompileShader(code, RL_COMPUTE_SHADER);
    computeProgram = rlLoadComputeShaderProgram(shader);
    UnloadFileText(code);   

    const int maxMirrorsSize = sizeof(Vector2)*MAX_MIRRORS*2;
    const int maxDistancesSize = sizeof(Vector2)*MAX_RAYS*MAX_BOUNCES;

    // Setup the SSBO's for the points and for the line segments.
    void *zeroMirrors = RL_MALLOC(maxMirrorsSize);
    void *zeroDistances = RL_MALLOC(maxDistancesSize);
    for (int i = 0; i < MAX_RAYS*MAX_BOUNCES; i++){ 
       ( (Vector2*)zeroDistances)[i] = { 1.0f, 1.0f };
    }
    mirrorSSBO = rlLoadShaderBuffer(maxMirrorsSize, zeroMirrors, RL_DYNAMIC_READ);
    distanceSSBO = rlLoadShaderBuffer(maxDistancesSize, zeroDistances, RL_DYNAMIC_COPY);

    // Setup the point VAO.
    Vector2 vertices[] = {
        { -0.86f, -0.5f },
        { 0.86f, -0.5f },
        { 0.0f,  1.0f }
    };
    pointVAO = rlLoadVertexArray();
    rlEnableVertexArray(pointVAO);
    rlEnableVertexAttribute(0);
    rlLoadVertexBuffer(vertices, sizeof(vertices),  false);
    rlSetVertexAttribute(0, 2, RL_FLOAT, false, 0, 0); 
    rlDisableVertexArray();
}
void Ray2Program::UpdateScene(bool center)
{
    rlUpdateShaderBuffer(mirrorSSBO, scene.mirrors.data(), scene.mirrors.size()*sizeof(Vector2), 0);
    if (center) {
        Vector2 sum = { 0, 0 };
        for (Vector2 point: scene.mirrors) {
            sum = Vector2Add(sum, point);
        }
        origin = Vector2Scale(sum, 1.0/scene.mirrors.size());
    }
}
void Ray2Program::ComputePass()
{
    if (scene.dirty) {
        UpdateScene(true);
        scene.dirty = false;
    }
    numBounces = min(numBounces, MAX_BOUNCES);
    numRays = min(numRays, MAX_RAYS);
    numWorkGroups = numRays / 1024 + 1;
    
    rlEnableShader(computeProgram);
    rlSetUniform(0, &numRays, SHADER_UNIFORM_INT, 1);
    rlSetUniform(1, &numBounces, SHADER_UNIFORM_INT, 1);
    rlSetUniform(2, &scene.numMirrors, SHADER_UNIFORM_INT, 1);
    rlSetUniform(3, &arcFocus, SHADER_UNIFORM_FLOAT, 1);

    float time = GetTime();
    rlSetUniform(4, &time, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(5, &origin, SHADER_UNIFORM_VEC2, 1);

    rlBindShaderBuffer(mirrorSSBO, 0);
    rlBindShaderBuffer(distanceSSBO, 1);

    rlComputeShaderDispatch(numWorkGroups, 1, 1);
    rlDisableShader();
}
void Ray2Program::RenderPass()
{
    rlEnableShader(renderProgram.id);
    rlSetUniform(0, &numRays, SHADER_UNIFORM_INT, 1);
    rlSetUniform(1, &numBounces, SHADER_UNIFORM_INT, 1);
    Vector4 colvec = ColorNormalize(color);
    rlSetUniform(2, &colvec, SHADER_UNIFORM_VEC3, 1);
    rlSetUniform(3, &falloff, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(4, &pointSize, SHADER_UNIFORM_FLOAT, 1);

    rlBindShaderBuffer(distanceSSBO, 0);

    // Draw the particles. Instancing will duplicate the vertices.
    rlEnableVertexArray(pointVAO);
    rlDrawVertexArrayInstanced(0, 3, numRays*numBounces);
    rlDisableVertexArray(); 
    rlDisableShader();
}

float Ray2Program::GetArcRadius()
{
    // Take the first point and compute arc radius like in the shader.
    Vector2 u = scene.mirrors[0];
    Vector2 v = scene.mirrors[1];
    Vector2 normal = Vector2Normalize({ -v.y+u.y, v.x-u.x });
    Vector2 center = Vector2Scale(normal, arcFocus);
    return Vector2Distance(center, u);
}