#include "Ray3Program.h"
#include "main.h"
#include <raymath.h>
#include <rlgl.h>

Ray3Program::Ray3Program()
{
    string vertPath = shaderPath + string("/ray3.vert");
    string fragPath = shaderPath + string("/ray3.frag");
    shader = LoadShader(vertPath.c_str(), fragPath.c_str());
    renderTexture = LoadRenderTexture(800, 800);
}
void Ray3Program::SetScene(Ray3Scene *_scene)
{
    scene = _scene;

    // Retrieve the packed data from the scene and send it to the GPU.
    // This data does not change and hence RL_STATIC_READ.
    vector<Vector4> vertices = _scene->GetMirrorVerticesPacked();
    vector<MirrorInfo> infos = _scene->GetMirrorInfosPacked();
    numMirrors = _scene->GetMirrorCount();

    if (mirrorVertexBuffer != 0) rlUnloadShaderBuffer(mirrorVertexBuffer);
    if (mirrorBuffer != 0) rlUnloadShaderBuffer(mirrorBuffer);
    mirrorVertexBuffer = rlLoadShaderBuffer(vertices.size()*sizeof(Vector4), vertices.data(), RL_STATIC_READ);
    mirrorBuffer = rlLoadShaderBuffer(infos.size()*sizeof(MirrorInfo), infos.data(), RL_STATIC_READ);
}
void Ray3Program::Draw()
{
    DrawTexturePro(renderTexture.texture, { 0, 0, (float)resolution, -(float)resolution }, { 0, 0, 800, 800 }, {}, 0, WHITE);
}
void Ray3Program::Render(Camera3D camera)
{
    if (!scene) return;
    if (dynamicResolution) {
        int targetfps = 60;
        int fps = GetFPS();
        int dfps = abs(fps-targetfps);
        float inc = 0.0001f;
        if (fps > targetfps+3) resolutionPercent += inc * dfps;
        if (fps < targetfps-3) resolutionPercent -= inc * dfps;
        resolutionPercent = Clamp(resolutionPercent, 0, 1);
    }

    BeginTextureMode(renderTexture);
    BeginMode3D(camera);

    // We only have to bind the buffers once.
    // They will persist for the entire drawing time.
    rlBindShaderBuffer(mirrorVertexBuffer, 0);
    rlBindShaderBuffer(mirrorBuffer, 1);
    SetShaderValue(shader, 0, &numMirrors, SHADER_UNIFORM_INT);
    float edgeThick = 0.01f*edgeThickness;
    SetShaderValue(shader, 1, &edgeThick, SHADER_UNIFORM_FLOAT);

    // Precompute inverted matrix so the fragment shader does not have to.
    Matrix proj = rlGetMatrixProjection();
    Matrix view = GetCameraMatrix(camera);
    Matrix mvp = MatrixMultiply(view, proj);
    Matrix invMvp = MatrixInvert(mvp);
    SetShaderValueMatrix(shader, 2, invMvp);

    SetShaderValue(shader, 3, &sphereFocus, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, 4, &numBounces, SHADER_UNIFORM_INT);
    resolution = (int)(800.0f*resolutionPercent);
    SetShaderValue(shader, 5, &resolution, SHADER_UNIFORM_INT);
    SetShaderValue(shader, 6, &falloff, SHADER_UNIFORM_FLOAT);
    int showMarkInt = showMark + 2*showEdgeMark + 4*showEdges;
    SetShaderValue(shader, 7, &showMarkInt, SHADER_UNIFORM_INT);
    float markSizeFl = 0.05f * markSize;
    SetShaderValue(shader, 8, &markSizeFl, SHADER_UNIFORM_FLOAT);

    Vector4 innerColorNorm = ColorNormalize(innerClearColor);
    SetShaderValue(shader, 9, &innerColorNorm, SHADER_UNIFORM_VEC4);
    Vector4 edgeColorNorm = ColorNormalize(edgeColor);
    SetShaderValue(shader, 10, &edgeColorNorm, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, 11, &lowerBounceLimit, SHADER_UNIFORM_INT);

    // Draw to custom render target first and then scale the result.
    rlViewport(0, 0, resolution, resolution);
    ClearBackground({ 0, 0, 0, 0 });
   
    // Draw a sphere that is the largest size that our models can be.
    rlSetCullFace(RL_CULL_FACE_FRONT);
    BeginShaderMode(shader);
    DrawSphere({}, 1.0f, WHITE);
    EndShaderMode();
    rlSetCullFace(RL_CULL_FACE_BACK);

    EndMode3D();
    EndTextureMode();
}
Ray3Scene* Ray3Program::GetScene()
{
    return scene;
}