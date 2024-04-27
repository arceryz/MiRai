#ifndef RAY3_H
#define RAY3_H

#include "Ray3Scene.h"
#include "Ray3Program.h"

class Ray3
{
private:
    Camera3D camera = {};
    Ray3Scene scene;
    Ray3Program program;
    Model model = {};
    Shader shader = {};

public:
    Ray3(): program(scene)
    {
        camera.fovy = 45.0f;
        camera.position = { 2, 2, 2 };
        camera.target = { 0, 0, 0 };
        camera.up = { 0, 1, 0 };
        camera.projection = CAMERA_PERSPECTIVE;
        model = LoadModelFromMesh(GenMeshCube(1, 1, 1));
        shader = LoadShader("Shaders/ray3.vert", "Shaders/ray3.frag");
        model.materials[0].shader = shader;
        scene.AddModel(model);
    }
    void Draw()
    {
        BeginMode3D(camera);
        //DrawModelWires(model, { 0, 0, 0 }, 0.3, BLUE);
        scene.DrawFaces(BLUE, GREEN);
        //scene.DebugDrawMesh(model.meshes[0]);
        UpdateCamera(&camera, CAMERA_ORBITAL);
        EndMode3D();
    }
};

#endif