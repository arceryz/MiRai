#ifndef RAY2_H
#define RAY2_H

#include "Ray2Scene.h"
#include "Ray2Program.h"
#include "raygui.h"
#include "Rayman.h"
#include "main.h"
#include <stdexcept>

// This class contains the Ray2 running/UI code.
class Ray2 
{
private:
    Ray2Scene scene;
    Ray2Program program;
    Camera2D camera = {};

    float numRaysFact = 0.5;
    int shape = 5;
    int currentShape = 5;
    Vector3 color = ColorToHSV({ 0, 255, 0, 255 });
    char zoomValueBuf[256] = "Edit (Z)";
    bool zoomValueEdit = false;

    float originAngle = 0.0f;
    float originRadius = 0.0f;
    bool drawInterface = true;
    float arcFocusPercent = 0.0f;
    float zoomTarget = 1.0f;
    bool centeredZoom = true;

public:
    Ray2(): program(scene)
    {
        scene.GenerateRegularPolygon(5);
        program.numRays = 1024;
        program.numBounces = 4;
        camera.offset = { 400, 400 };
        camera.target = { 400, 400 };
        camera.zoom = 1.0;

        //scene.GenerateTriangle(PI/2.0, PI/4, PI/4);
        //scene.GenerateTriangle(PI/2, PI/3, PI/6);
        //scene.GenerateTriangle(2*PI/3, PI/6, PI/6);
        //scene.GenerateNiceRhombus();
        scene.GenerateTokarsky();
    };
    void RenderUpdate()
    {
        float delta = GetFrameTime();
        program.arcFocus = (arcFocusPercent > 0 ? 1: -1) * powf(1-abs(arcFocusPercent), 8) * (FOCUS_INF);
        program.numRays = (int)(pow(numRaysFact, 8) * MAX_RAYS);
        program.color = ColorFromHSV(color.x, color.y, color.z);

        if (currentShape != shape) {
            currentShape = shape;
            scene.GenerateRegularPolygon(shape);
        }
        originAngle += 45.0*delta*(IsKeyDown(KEY_D) ? -1.0: (IsKeyDown(KEY_A) ? +1.0: 0.0));
        originRadius = Clamp(originRadius+0.5*delta*(IsKeyDown(KEY_W) ? 1.0: (IsKeyDown(KEY_S) ? -1.0: 0.0)), 0, 10);
        program.origin = { originRadius*cosf(DEG2RAD*originAngle), originRadius*sinf(DEG2RAD*originAngle) };

        // Set camera offset.
        DragCameraUpdate(camera, zoomTarget, centeredZoom, 0.01, 16.0, 0.4, 10.0, 200.0);
        Vector2 screen = GetWorldToScreen2D({ 400, 400 }, camera);
        Vector2 rel = { -(400-screen.x)/400.0, (400-screen.y)/400.0 };
        program.cameraOffset = rel;

        //float scroll = GetMouseWheelMove();
        program.zoom = camera.zoom;
        program.color = mainEdgeColor;
        program.ComputePass();
    }

    void CenterView()
    {
        camera.target = { 400, 400 };
        camera.offset = { 400, 400 };
        centeredZoom = true;
    }
    void GenerateRandom()
    {
        scene.GenerateRandomCirclePolygon(shape, 20);
    }
    void SetImageAlignCurvature()
    {
        int n = 1;

        float alpha = (2*PI) / shape;
        float denom = (cos(alpha/2)-sinf((n+1)*alpha/2));
        if (denom == 0.0) return;
        float lambda = (1-sinf(n*alpha/2)) / denom;
        SetLambda(lambda);
    }
    void SetInternalAlignCurvature()
    {
        float beta = (2*PI)/3.0;
        float alpha = PI - (2*PI)/shape;
        
        float sinab = sin((alpha-beta)/2.0);
        if (sinab == 0) return;
        float lambda = cos(beta/2.0) / sinab;
        SetLambda(lambda);
    }
    void SetLambda(float lambda)
    {
        float f = (lambda > 0 ? 1: -1) * (1.0-powf(abs(lambda) / FOCUS_INF, 1.0/8.0));   
        arcFocusPercent = f;
    }
    void SetFlat()
    {
        arcFocusPercent = 0;
    }
    void DrawGUI()
    {
        if (IsKeyPressed(KEY_C)) CenterView();
        if (IsKeyPressed(KEY_R)) GenerateRandom();
        if (IsKeyPressed(KEY_F)) SetFlat();
        if (IsKeyPressed(KEY_I)) SetImageAlignCurvature();
        if (IsKeyPressed(KEY_O)) SetInternalAlignCurvature();
        
        if (focusMode) {
             GuiSlider({ 200, 10, 400, 10 }, "Arc Focus", TextFormat("%.1f", program.arcFocus), &arcFocusPercent, -0.999, 1);  
             return;
        }
        DrawText(TextFormat("N=%d", program.numRays), 10, 30, 20, DARKGRAY);
        DrawFPS(10, 10);

        float ox = 170;
        float oy = 15;
        GuiSlider({ ox, oy, 200, 10 }, "Rays%", TextFormat("%3.1f", 100*numRaysFact), &numRaysFact, 0, 1);
        GuiSlider({ ox, oy+20, 200, 10 }, "Focus", TextFormat("%.2f (r=%.3f)", program.arcFocus, program.GetArcRadius()), &arcFocusPercent, -0.999, 0.999);
        GuiSlider({ ox+270, oy, 100, 10 }, "Zoom", TextFormat("%.1f", zoomTarget), &zoomTarget, 0.1, 16);
        GuiCheckBox({ ox+315, oy+15, 20, 20 }, "Draw Lines", &drawInterface);
        GuiSpinner({ ox, oy+40, 80, 15 }, "Num Bounces ", &program.numBounces, 1, MAX_BOUNCES, false);
        GuiSpinner({ ox, oy+60, 80, 15 }, "Start Bounce ", &program.bounceStart, 0, MAX_BOUNCES, false);
        GuiSpinner({ ox+130, oy+40, 80, 15 }, "Shape ", &shape, 3, 100, false);
        if (GuiTextBox({ ox+395, oy-2, 50, 14 }, zoomValueBuf, 256, zoomValueEdit)) {
            zoomValueEdit = false;
            try {
                zoomTarget = stof(zoomValueBuf);
            }
            catch(std::invalid_argument exc) {
            }
            strcpy(zoomValueBuf, "Edit (Z)");
        }
        if (IsKeyPressed(KEY_Z)) {
            strcpy(zoomValueBuf, "");
            zoomValueEdit = true;
        }
    
        ox = 660;
        oy = 13;
        GuiSlider({ ox, oy, 100, 10 }, "Falloff", TextFormat("%.2f", program.falloff), &program.falloff, 0.001, 0.5);
        GuiSlider({ ox, oy+20, 100, 10 }, "Point Size", TextFormat("%.2f", program.pointSize), &program.pointSize, 0.3, 3);
        GuiSlider({ ox, oy+40, 100, 10 }, "Corner Highlight", TextFormat("%.2f", program.cornerFactor), &program.cornerFactor, 0, 1);
        GuiSlider({ ox, oy+60, 100, 10 }, "Corner Size", TextFormat("%.2f", program.cornerSize), &program.cornerSize, 0, 1);
        GuiSlider({ ox, oy+80, 100, 10 }, "Target Size", TextFormat("%.2f", program.targetSize), &program.targetSize, 0, 0.5);
        oy += 30;
        if (GuiButton({ ox+40, oy+70, 80, 20 }, "Center (C) ")) CenterView();
        if (GuiButton({ ox+40, oy+100, 80, 20 }, "Random (R)")) GenerateRandom();
        if (GuiButton({ ox+40, oy+130, 80, 20 }, "Flat (F)")) SetFlat();
        if (GuiButton({ ox+40, oy+160, 80, 20 }, "Image Align (I)")) SetImageAlignCurvature();
        if (GuiButton({ ox+40, oy+190, 80, 20 }, "Angle Align (O)")) SetInternalAlignCurvature();

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
            program.targetPosition = program.InvTransform(mouse);
        }
    }
    void DrawContent() 
    {
        BeginMode2D(camera);
        program.RenderPass();
        if (drawInterface) program.InterfacePass();
        EndMode2D();
    }
};

#endif