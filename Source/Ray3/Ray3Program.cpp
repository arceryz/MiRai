#include "Ray3Program.h"
#include <rlgl.h>

Ray3Program::Ray3Program()
{
    shader = LoadShader("Shaders/ray3.vert", "Shaders/ray3.frag");
}
void Ray3Program::SetScene(Ray3Scene *_scene)
{
    scene = _scene;

    // Retrieve the packed data from the scene and send it to the GPU.
    // This data does not change and hence RL_STATIC_READ.
    vector<Vector4> vertices = _scene->GetMirrorVerticesPacked();
    vector<Vector4> normals = _scene->GetMirrorNormalsPacked();
    vector<int> sizes = _scene->GetMirrorSizesPacked();
    numMirrors = _scene->GetNumMirrors();
    printf("%d Mirrors\n", numMirrors);
    
    int offset = 0;
    for (int i = 0; i < numMirrors; i++) {
        Vector4 normal = normals[i];
        int size = sizes[i];
        printf("Face %d size %d normal (%1.1f %1.1f %1.1f)\n", i, size, normal.x, normal.y, normal.z);
        for (int j = 0; j < size; j++) { 
            Vector4 vert = vertices[j+offset];
            printf("Vertex %d (%1.1f %1.1f %1.1f)\n", j, vert.x, vert.y, vert.z);
        }
        offset += size;
    }
    mirrorVertexBuffer = rlLoadShaderBuffer(vertices.size()*sizeof(Vector4), vertices.data(), RL_STATIC_READ);
    mirrorNormalBuffer = rlLoadShaderBuffer(normals.size()*sizeof(Vector4), normals.data(), RL_STATIC_READ);
    mirrorSizeBuffer = rlLoadShaderBuffer(sizes.size()*sizeof(int), sizes.data(), RL_STATIC_READ);
}
void Ray3Program::Draw()
{
    if (!scene) return;

    // We only have to bind the buffers once.
    // They will persist for the entire drawing time.
    rlBindShaderBuffer(mirrorVertexBuffer, 0);
    rlBindShaderBuffer(mirrorNormalBuffer, 1);
    rlBindShaderBuffer(mirrorSizeBuffer, 2);
    SetShaderValue(shader, 0, &numMirrors, SHADER_UNIFORM_INT);
    float edgeThick = 0.001f*edgeThickness;
    SetShaderValue(shader, 1, &edgeThick, SHADER_UNIFORM_FLOAT);

    // The model uses the same shader, but we use the Raylib method
    // of drawing models since it is easiest.
    model.materials->shader = shader;
    rlSetCullFace(RL_CULL_FACE_FRONT);
    DrawModel(model, {}, 1, WHITE);
    rlSetCullFace(RL_CULL_FACE_BACK);
}