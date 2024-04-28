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
        model = LoadModel("Models/Dodecahedron.obj");
        shader = LoadShader("Shaders/ray3.vert", "Shaders/ray3.frag");
        model.materials[0].shader = shader;
        scene.AddMirrorModel(model, true);
    }
    void Draw()
    {
        BeginMode3D(camera);
        vector<Color> colors = {
            RED,
            GREEN,
            BLUE,
            ORANGE,
            MAGENTA,
            GRAY
        };
        scene.DrawMirrors(colors, 0.9);

        UpdateCamera(&camera, CAMERA_ORBITAL);
        EndMode3D();
    }
};

#endif