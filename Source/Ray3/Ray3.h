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
    bool doRotate = true;
    bool showMesh = true;
public:
    Ray3()
    {
        camera.fovy = 45.0f;
        camera.position = { 3, 2, 3 };
        camera.target = { 0, 0, 0 };
        camera.up = { 0, 1, 0 };
        camera.projection = CAMERA_PERSPECTIVE;

        model = LoadModel("Models/Dodecahedron.obj");
        scene.name = "Cube";
        scene.AddMirrorModel(model, true);
        program.model = model;
        program.SetScene(&scene);
    }
    void Draw()
    {
        if (doRotate) UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginMode3D(camera);
        vector<Color> colors = {
            RED,
            GREEN,
            BLUE,
            ORANGE,
            MAGENTA,
            GRAY
        };
        if (showMesh) scene.DrawMirrors(colors, 0.99, 0.95);
        program.Draw();
        // rlSetCullFace(RL_CULL_FACE_FRONT);
        // DrawSphereEx({}, 1.0f, 32, 32, Fade(RED, 0.3f));
        // rlSetCullFace(RL_CULL_FACE_BACK);
        EndMode3D();

        DrawFPS(10, 10);

        float spd = GetFrameTime();
        if (IsKeyDown(KEY_UP)) camera.position.y += spd;
        if (IsKeyDown(KEY_DOWN)) camera.position.y -= spd;
        if (IsKeyPressed(KEY_SPACE)) doRotate = !doRotate;

        if (GuiButton({ 700, 10, 80, 20 }, TextFormat(showMesh ? "Hide Mesh": "Show Mesh"))) showMesh = !showMesh;
        GuiSlider({ 170, 10, 200, 10 }, "Edge Size", TextFormat("%.1f", program.edgeThickness), &program.edgeThickness, 0.1, 10.0);  
    }
};

#endif