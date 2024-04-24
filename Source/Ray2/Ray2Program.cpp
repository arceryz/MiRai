#include <stdlib.h>
#include <algorithm>
#include "Ray2Program.h"
#include <rlgl.h>
using namespace std;

Ray2Program::Ray2Program()
{
    renderProgram = LoadShader("Shaders/ray2.vert", "Shaders/ray2.frag");
    
    // Compute shader.
    char *code = LoadFileText("Shaders/ray2.comp");
    int shader = rlCompileShader(code, RL_COMPUTE_SHADER);
    computeProgram = rlLoadComputeShaderProgram(shader);
    UnloadFileText(code);   

    const int maxPointsSize = sizeof(Vector2)*MAX_POINTS;
    const int maxDistancesSize = sizeof(float)*MAX_RAYS*MAX_BOUNCES;

    // Setup the SSBO's for the points and for the line segments.
    void *zeropoints = RL_MALLOC(maxPointsSize);
    void *zerodistances = RL_MALLOC(maxDistancesSize);
    pointSSBO = rlLoadShaderBuffer(maxPointsSize, zeropoints, RL_DYNAMIC_READ);
    distanceSSBO = rlLoadShaderBuffer(maxDistancesSize, zerodistances, RL_DYNAMIC_COPY);

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
void Ray2Program::UpdatePoints(int num, Vector2 *points)
{
    rlUpdateShaderBuffer(pointSSBO, points, num*sizeof(Vector2), 0);
    numPoints = num;
}
void Ray2Program::ComputePass()
{
    numBounces = min(numBounces, MAX_BOUNCES);
    numRays = min(numRays, MAX_RAYS);
    numWorkGroups = numRays / 1024 + 1;
    
    rlEnableShader(computeProgram);
    rlSetUniform(0, &numRays, SHADER_UNIFORM_INT, 1);
    rlSetUniform(1, &numBounces, SHADER_UNIFORM_INT, 1);
    rlSetUniform(2, &numPoints, SHADER_UNIFORM_INT, 1);
    rlSetUniform(3, &arcFocus, SHADER_UNIFORM_FLOAT, 1);
    rlBindShaderBuffer(pointSSBO, 0);
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

    rlBindShaderBuffer(pointSSBO, 0);
    rlBindShaderBuffer(distanceSSBO, 1);

    // Draw the particles. Instancing will duplicate the vertices.
    rlEnableVertexArray(pointVAO);
    rlDrawVertexArrayInstanced(0, 3, numRays*numBounces);
    rlDisableVertexArray(); 
    rlDisableShader();
}