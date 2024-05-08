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
    float rotY = 0;
    float radius = 3;

    bool showMesh = false;
    float sphereFocusPercent = 1.0f;
    float numBouncesFl = 3;

public:
    Ray3()
    {
        camera.fovy = 45.0f;
        camera.position = {};
        camera.target = { 0, 0, 0 };
        camera.up = { 0, 1, 0 };
        camera.projection = CAMERA_PERSPECTIVE;

        model = LoadModel("Models/Cube.obj");
        scene.name = "Cube";
        scene.AddMirrorModel(model, true);
        program.model = model;
        program.SetScene(&scene);
    }
    void Draw()
    {
        float dt = GetFrameTime();
        program.sphereFocus = 1+powf(sphereFocusPercent, 8) * (SPHERE_FOCUS_INF-1);
        program.numBounces = (int)numBouncesFl;

        BeginMode3D(camera);
        vector<Color> colors = {
            RED,
            GREEN,
            BLUE,
            ORANGE,
            MAGENTA,
            GRAY
        };
        program.Draw(camera);
        if (showMesh) scene.DrawMirrors(colors, 0.99, 0.95);

        // rlSetCullFace(RL_CULL_FACE_FRONT);
        // DrawSphereEx({}, 1.0f, 32, 32, Fade(RED, 0.3f));
        // rlSetCullFace(RL_CULL_FACE_BACK);
        EndMode3D();

        DrawFPS(10, 10);

        float spd = GetFrameTime();
        if (IsKeyDown(KEY_UP)) camera.position.y += spd;
        if (IsKeyDown(KEY_DOWN)) camera.position.y -= spd;
        if (IsKeyPressed(KEY_SPACE)) doRotate = !doRotate;
        camera.position.x = radius * cosf(DEG2RAD*rotY);
        camera.position.z = radius * sinf(DEG2RAD*rotY);
        float scroll = GetMouseWheelMove();
        radius = Clamp(radius+scroll, 1, 10);
        rotY += (float)(IsKeyDown(KEY_LEFT)-IsKeyDown(KEY_RIGHT)) * 45.0f*dt;

        if (GuiButton({ 700, 10, 80, 20 }, TextFormat(showMesh ? "Hide Mesh": "Show Mesh"))) showMesh = !showMesh;
        GuiSlider({ 170, 10, 200, 10 }, "Edge Size", TextFormat("%.1f", program.edgeThickness), &program.edgeThickness, 0.1, 10.0);
        GuiSlider({ 170, 30, 200, 10 }, "Sphere Focus", TextFormat("%.1f", program.sphereFocus), &sphereFocusPercent, 0, 1);  
        GuiSlider({ 170, 50, 200, 10 }, "Num Bounces", TextFormat("%d", program.numBounces), &numBouncesFl, 0, 20);
    }
};

#endif