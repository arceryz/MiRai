#include "Ray3Program.h"
#include <raymath.h>
#include <rlgl.h>

Ray3Program::Ray3Program()
{
    shader = LoadShader("Shaders/ray3.vert", "Shaders/ray3testing.frag");
}
void Ray3Program::SetScene(Ray3Scene *_scene)
{
    scene = _scene;

    // Retrieve the packed data from the scene and send it to the GPU.
    // This data does not change and hence RL_STATIC_READ.
    vector<Vector4> vertices = _scene->GetMirrorVerticesPacked();
    vector<MirrorInfo> infos = _scene->GetMirrorInfosPacked();
    numMirrors = _scene->GetMirrorCount();

    mirrorVertexBuffer = rlLoadShaderBuffer(vertices.size()*sizeof(Vector4), vertices.data(), RL_STATIC_READ);
    mirrorBuffer = rlLoadShaderBuffer(infos.size()*sizeof(MirrorInfo), infos.data(), RL_STATIC_READ);
}
void Ray3Program::Draw(Camera3D camera)
{
    if (!scene) return;

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
    Matrix mvp = MatrixMultiply(MatrixMultiply(model.transform, view), proj);
    Matrix invMvp = MatrixInvert(mvp);
    SetShaderValueMatrix(shader, 2, invMvp);

    SetShaderValue(shader, 3, &sphereFocus, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, 4, &numBounces, SHADER_UNIFORM_INT);

    // The model uses the same shader, but we use the Raylib method
    // of drawing models since it is easiest.
    model.materials->shader = shader;
    rlSetCullFace(RL_CULL_FACE_FRONT);
    BeginShaderMode(shader);
    DrawSphere({}, 2.0f, WHITE);
    EndShaderMode();
    rlSetCullFace(RL_CULL_FACE_BACK);
}