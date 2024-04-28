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
    Model model;
public:
    Ray3()
    {
        camera.fovy = 45.0f;
        camera.position = { 3, 3, 3 };
        camera.target = { 0, 0, 0 };
        camera.up = { 0, 1, 0 };
        camera.projection = CAMERA_PERSPECTIVE;

        model = LoadModel("Models/Cube.obj");
        scene.name = "Cube";
        scene.AddMirrorModel(model, true);
        program.model = model;
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
        //program.Draw();

        UpdateCamera(&camera, CAMERA_ORBITAL);
        EndMode3D();
    }
};

#endif