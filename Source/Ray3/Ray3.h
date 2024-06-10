#ifndef RAY3_H
#define RAY3_H

#include "Ray3Scene.h"
#include "Ray3Program.h"
#include "Rayman.h"
#include "XCamera.h"

class Ray3
{
private:
    XCamera *camera;
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
    float sphereFocusPercent = 0.0f;
    float numBouncesFl = 10;
    int sceneIndex = 0;
    bool extremeMode = false;
    bool setInnerColor = false;

public:
    Ray3()
    {
        camera = new XCamera();
        camera->internal.position = { 0, 0, 4 };

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
        if (scenes.size() == 0) {
            Ray3Scene scene;
            scene.name = "Empty Scene";
            scenes.push_back(scene);
        }
        program.SetScene(&scenes[0]);
    }
    void Draw()
    {
        { // Update Pass.
            float dt = GetFrameTime();
            program.sphereFocus = (sphereFocusPercent > 0 ? 1: -1) * powf(1-abs(sphereFocusPercent), 8) * (SPHERE_FOCUS_INF);
            program.numBounces = (int)numBouncesFl;
            Ray3Scene *activeScene = &scenes[sceneIndex];
            if (program.GetScene() != activeScene) { program.SetScene(activeScene); };
            if (IsKeyPressed(KEY_H)) hideGui = !hideGui;

            float scroll = -GetMouseWheelMove();
            float radialSpeed = 10.0f;
            float hspeed = 35.0f;
            float vspeed = 20.0f;
            camera->SetRadius(camera->GetRadius()*(1.0+scroll*dt*radialSpeed));
            if (!camera->perspective) camera->orthoSize = camera->orthoSize*(1.0+scroll*dt*radialSpeed);
            if (IsKeyDown(KEY_A)) camera->RotateH(-hspeed*dt);
            if (IsKeyDown(KEY_D)) camera->RotateH(+hspeed*dt);
            if (IsKeyDown(KEY_W)) camera->RotateV(vspeed*dt);
            if (IsKeyDown(KEY_S)) camera->RotateV(-vspeed*dt);
            if (IsKeyPressed(KEY_E)) {
                // Align to the first polygon of the scene.
                Ray3Scene &scene = *program.GetScene();
                Polygon &poly = scene.mirrors[0];
                Vector3 normalvec = Vector3Scale(poly.normal, camera->GetRadius());
                camera->internal.position = normalvec;
            }
            if (IsKeyPressed(KEY_R)) {
                camera->SetRadius(0);
                camera->orthoSize = 2;
            }
            if (IsKeyPressed(KEY_F)) sphereFocusPercent = 0;
        }

        { // Render Pass.
            camera->Begin();
            program.Draw(camera->internal);
            if (showMesh) program.GetScene()->DrawMirrors(0.99, 0.95);
            EndMode3D();
        }

        // GUI Pass.
        if (!hideGui) {
            DrawFPS(10, 10);
            if (GuiButton({ 700, 10, 80, 20 }, TextFormat(showMesh ? "Mesh Visible": "Mesh Hidden"))) showMesh = !showMesh;
            if (GuiButton({ 700, 40, 80, 20 }, TextFormat(program.dynamicResolution ? "Dynamic Res": "Static Res"))) program.dynamicResolution = !program.dynamicResolution;
            if (GuiButton({ 700, 70, 80, 20 }, TextFormat(camera->perspective ? "Perspective": "Orthographic"))) { camera->perspective = !camera->perspective; }
            if (GuiButton({ 700, 100, 80, 20 }, TextFormat(extremeMode ? "High Bounce": "Low Bounce"))) extremeMode = !extremeMode;
            if (GuiButton({ 680, 130, 100, 20 }, TextFormat(program.showMark ? "Mark Visible": "Mark Hidden"))) program.showMark = !program.showMark;
            if (GuiButton({ 680, 160, 100, 20 }, TextFormat(program.showEdgeMark ? "Edge Mark Visible": "Edge Mark Hidden"))) program.showEdgeMark = !program.showEdgeMark;
            if (GuiButton({ 680, 190, 100, 20 }, TextFormat(program.showEdges ? "Edges Visible": "Edges Hidden"))) program.showEdges = !program.showEdges;
            
            if (GuiButton({ 680, 220, 100, 20 }, "Inner Color")) setInnerColor = !setInnerColor;
            if (setInnerColor) GuiColorPicker({ 550, 220, 100, 100 }, "", &program.innerClearColor);
            program.innerClearColor.a = 255;
            
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

            GuiSlider({ 170, 10, 200, 10 }, "Edge Size", TextFormat("%.1f", program.edgeThickness), &program.edgeThickness, 0.1f, 10.0);
            GuiSlider({ 170, 30, 200, 10 }, "Sphere Focus", TextFormat("%.1f", program.sphereFocus), &sphereFocusPercent, -0.999, 1);  
            GuiSlider({ 170, 50, 200, 10 }, "Num Bounces", TextFormat("%d", program.numBounces), &numBouncesFl, 0, extremeMode ? 100: 20);
            GuiSlider({ 170+300, 10, 190, 10 }, "Resolution%", TextFormat("%.2f", program.resolutionPercent), &program.resolutionPercent, 0, 1);
            GuiSlider({ 170+300, 30, 190, 10 }, "Falloff", TextFormat("%.2f", program.falloff), &program.falloff, 0, 0.999f);
            GuiSlider({ 170+300, 50, 190, 10 }, "Mark Size", TextFormat("%.2f", program.markSize), &program.markSize, 0.1f, 10.0f);
            GuiSlider({ 170+300, 70, 190, 10 }, "FOV", TextFormat("%.2f", camera->fovy), &camera->fovy, 10.0, 145.0);
        }
        else {
             GuiSlider({ 200, 10, 400, 10 }, "Sphere Focus", TextFormat("%.1f", program.sphereFocus), &sphereFocusPercent, -0.999, 1);  
        }
    }
};

#endif