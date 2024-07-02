#include <stdlib.h>
#include <algorithm>
#include "Ray2Program.h"
#include <raymath.h>
#include <rlgl.h>
#include "main.h"
using namespace std;

Ray2Program::Ray2Program(Ray2Scene &_scene)
: scene(_scene)
{
    string vertPath = shaderPath + string("/ray2.vert");
    string fragPath = shaderPath + string("/ray2.frag");
    string compPath = shaderPath + string("/ray2.comp");
    renderProgram = LoadShader(vertPath.c_str(), fragPath.c_str());
    
    // Compute shader.
    char *code = LoadFileText(compPath.c_str());
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
    rlSetUniform(6, &bounceStart, SHADER_UNIFORM_INT, 1);
    rlSetUniform(7, &cornerSize, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(8, &targetPosition, SHADER_UNIFORM_VEC2, 1);
    rlSetUniform(9, &targetSize, SHADER_UNIFORM_FLOAT, 1);

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
    rlSetUniform(5, &zoom, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(6, &origin, SHADER_UNIFORM_VEC2, 1);
    rlSetUniform(7, &cornerFactor, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(8, &cameraOffset, SHADER_UNIFORM_VEC2, 1);
    rlBindShaderBuffer(distanceSSBO, 0);

    // Draw the particles. Instancing will duplicate the vertices.
    rlEnableVertexArray(pointVAO);
    rlDrawVertexArrayInstanced(0, 3, numRays*numBounces);
    rlDisableVertexArray(); 
    rlDisableShader();
}

void Ray2Program::InterfacePass()
{
    Vector2 originWorld = Transform(origin);
    Vector2 centerWorld = Transform({ 0, 0 });
    Vector2 radiusWorld = Transform({ 0, 1 });
    DrawCircleLinesV(centerWorld, Vector2Length(Vector2Subtract(radiusWorld, centerWorld)), Fade(RED, 0.5));
    DrawCircleV(originWorld, 1.0, RED);

    float r = 0.3;
    for (int i = 0; i < 10; i++) {
        float angle = ((float)i)/10.0 * 2*PI;
        Vector2 world = Transform(Vector2Add(origin, { r*cosf(angle), r*sinf(angle) }));
        DrawLineV(originWorld, world, Fade(RED, 0.5));
    }
}

Vector2 Ray2Program::Transform(Vector2 local)
{
    Vector2 pos = Vector2Scale(local, 0.1);
    return {
        (float)((pos.x+1.0)/2.0*800.0f),
        (float)((-pos.y+1.0)/2.0*800.0f)
    };
}

Vector2 Ray2Program::InvTransform(Vector2 screen)
{
    return {
        (float)(screen.x * 2.0f / 800.0f - 1.0f) * 10.0f,
        -(float)(screen.y * 2.0f / 800.0f - 1.0f) * 10.0f
    };
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