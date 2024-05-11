#ifndef RAY3_H
#define RAY3_H

#include "Ray3Scene.h"
#include "Ray3Program.h"
#include "Rayman.h"

class Ray3
{
private:
    Camera3D camera = {};
    vector<Ray3Scene> scenes;
    Ray3Program program;
    Model model;

    // GUI settings.
    bool doRotate = true;
    float rotY = 0;
    float radius = 3;
    float orthoSize = 2.0f;
    bool showMesh = false;
    bool hideGui = false;
    bool selectingModel = false;

    // Parameter subtitutes.
    float sphereFocusPercent = 1.0f;
    float numBouncesFl = 10;
    int sceneIndex = 0;
    bool extremeMode = false;

public:
    Ray3()
    {
        camera.fovy = 45.0f;
        camera.position = {};
        camera.target = { 0, 0, 0 };
        camera.up = { 0, 1, 0 };
        camera.projection = CAMERA_PERSPECTIVE;

        FilePathList modelFiles = LoadDirectoryFiles("Models");
        for (int i = 0; i < modelFiles.count; i++) {
            const char *path = modelFiles.paths[i];
            const char *basename = GetFileNameWithoutExt(path);
            printf("%d. Loading \"%s\"\n", i+1, basename);

            Model model = LoadModel(modelFiles.paths[i]);
            Ray3Scene scene;
            scene.name = string(basename);
            scene.AddMirrorModel(model, true);
            scene.NormalizeRadius();
            scene.Centralize();
            scenes.push_back(scene);
        }
        program.SetScene(&scenes[0]);
    }
    void Draw()
    {
        float dt = GetFrameTime();
        program.sphereFocus = powf(sphereFocusPercent, 8) * (SPHERE_FOCUS_INF-1);
        program.numBounces = (int)numBouncesFl;
        Ray3Scene *activeScene = &scenes[sceneIndex];
        if (program.GetScene() != activeScene) { program.SetScene(activeScene); };

        float spd = GetFrameTime();
        if (IsKeyDown(KEY_UP)) camera.position.y += spd;
        if (IsKeyDown(KEY_DOWN)) camera.position.y -= spd;
        if (IsKeyPressed(KEY_SPACE)) doRotate = !doRotate;
        if (IsKeyPressed(KEY_H)) hideGui = !hideGui;
        camera.position.x = radius * cosf(DEG2RAD*rotY);
        camera.position.z = radius * sinf(DEG2RAD*rotY);
        float scroll = GetMouseWheelMove();
        if (camera.projection == CAMERA_PERSPECTIVE) {
            radius = Clamp(radius+scroll, 1, 10);
            camera.fovy = 45.0f;
        } 
        else {
            orthoSize = Clamp(orthoSize+scroll, 0.1, 10);
            camera.fovy = orthoSize;
        }
        rotY += (float)(IsKeyDown(KEY_LEFT)-IsKeyDown(KEY_RIGHT)) * 15.0f*dt;

        BeginMode3D(camera);
        program.Draw(camera);
        if (showMesh) program.GetScene()->DrawMirrors(0.99, 0.95);
        EndMode3D();

        if (!hideGui) {
            DrawFPS(10, 10);
            if (GuiButton({ 700, 10, 80, 20 }, TextFormat(showMesh ? "Mesh Visible": "Mesh Hidden"))) showMesh = !showMesh;
            if (GuiButton({ 700, 40, 80, 20 }, TextFormat(program.dynamicResolution ? "Dynamic Res": "Static Res"))) program.dynamicResolution = !program.dynamicResolution;
            if (GuiButton({ 700, 70, 80, 20 }, TextFormat(camera.projection == CAMERA_PERSPECTIVE ? "Perspective": "Orthographic"))) {
                camera.projection = camera.projection == CAMERA_PERSPECTIVE ? CAMERA_ORTHOGRAPHIC : CAMERA_PERSPECTIVE;
            }
            if (GuiButton({ 700, 100, 80, 20 }, TextFormat(extremeMode ? "High Bounce": "Low Bounce"))) extremeMode = !extremeMode;
            if (GuiButton({ 10, 70, 80, 20 }, selectingModel ? "> > >": program.GetScene()->name.c_str()))  selectingModel = !selectingModel;
            if (selectingModel) {
                for (int i = 0; i < scenes.size(); i++) {
                    Ray3Scene &scene = scenes[i];
                    if (GuiButton({ 10, 70.0f+25*(i+1), 120, 20 }, scene.name.c_str())) {
                        sceneIndex = i;
                        selectingModel = false;
                    }
                }
            }

            GuiSlider({ 170, 10, 200, 10 }, "Edge Size", TextFormat("%.1f", program.edgeThickness), &program.edgeThickness, 0.1, 10.0);
            GuiSlider({ 170, 30, 200, 10 }, "Sphere Focus", TextFormat("%.1f", program.sphereFocus), &sphereFocusPercent, 0, 1);  
            GuiSlider({ 170, 50, 200, 10 }, "Num Bounces", TextFormat("%d", program.numBounces), &numBouncesFl, 0, extremeMode ? 100: 20);
            GuiSlider({ 170+300, 10, 200, 10 }, "Resolution%", TextFormat("%.2f", program.resolutionPercent), &program.resolutionPercent, 0, 1);
            GuiSlider({ 170+300, 30, 200, 10 }, "Falloff", TextFormat("%.2f", program.falloff), &program.falloff, 0, 0.999f);
        }
    }
};

#endif