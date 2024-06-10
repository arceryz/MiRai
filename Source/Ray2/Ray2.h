#ifndef RAY2_H
#define RAY2_H

#include "Ray2Scene.h"
#include "Ray2Program.h"
#include "raygui.h"
#include "main.h"

// This class contains the Ray2 running/UI code.
class Ray2 
{
public:
    float originAngle = 0.0f;
    float originRadius = 0.0f;
    bool drawInterface = true;
    bool hideGui = false;

    Ray2(): program(scene)
    {
        scene.GenerateRegularPolygon(5);
        program.numRays = 1024;
        program.numBounces = 4;
        camera.offset = { 400, 400 };
        camera.target = { 400, 400 };
        camera.zoom = 1.0;
    };
    void RenderUpdate()
    {
        float delta = GetFrameTime();
        program.numRays = (int)(pow(numRaysFact, 8) * MAX_RAYS);
        program.color = ColorFromHSV(color.x, color.y, color.z);
        program.arcFocus = (hyperbolic ? -1 : 1) * (pow(focusFact, 5) * (ARC_FOCUS_INF-1));

        if (currentShape != shape) {
            currentShape = shape;
            scene.GenerateRegularPolygon(shape);
        }
        if (IsKeyPressed(KEY_R)) {
            scene.GenerateRandomCirclePolygon(5, 20);
        }
        if (IsKeyPressed(KEY_H)) hideGui = !hideGui;

        originAngle += 45.0*delta*(IsKeyDown(KEY_D) ? -1.0: (IsKeyDown(KEY_A) ? +1.0: 0.0));
        originRadius = Clamp(originRadius+0.5*delta*(IsKeyDown(KEY_W) ? 1.0: (IsKeyDown(KEY_S) ? -1.0: 0.0)), 0, 1);
        program.origin = { originRadius*cosf(DEG2RAD*originAngle), originRadius*sinf(DEG2RAD*originAngle) };

        float scroll = GetMouseWheelMove();
        program.zoom = Clamp(program.zoom * (1.0 + scroll*0.2), 0.25, 4.0);
        program.color = mainEdgeColor;
        program.ComputePass();
    }
    void DrawGUI()
    {
        if (!hideGui)
        { // Gui Pass.
            DrawText(TextFormat("N=%d", program.numRays), 10, 30, 20, DARKGRAY);
            DrawFPS(10, 10);

            float ox = 170;
            float oy = 15;
            GuiSlider({ ox, oy, 200, 10 }, "Rays%", TextFormat("%3.1f", 100*numRaysFact), &numRaysFact, 0, 1);
            GuiSlider({ ox, oy+20, 200, 10 }, "Focus", TextFormat("%.2f (r=%.3f)", -program.arcFocus, program.GetArcRadius()), &focusFact, 0.001, 1);
            GuiSlider({ ox+270, oy, 100, 10 }, "Zoom", TextFormat("%.1f", program.zoom), &program.zoom, 0.25, 4);
            GuiCheckBox({ ox+315, oy+15, 20, 20 }, "Hyperbolic", &hyperbolic);
            GuiCheckBox({ ox+315, oy+40, 20, 20 }, "Draw Lines", &drawInterface);
            GuiSpinner({ ox, oy+40, 80, 15 }, "Num Bounces ", &program.numBounces, 1, MAX_BOUNCES, false);
            GuiSpinner({ ox+130, oy+40, 80, 15 }, "Shape ", &shape, 3, 10, false);
        
            ox = 660;
            oy = 13;
            GuiSlider({ ox, oy, 100, 10 }, "Falloff", TextFormat("%.2f", program.falloff), &program.falloff, 0.001, 1);
            GuiSlider({ ox, oy+20, 100, 10 }, "Point Size", TextFormat("%.2f", program.pointSize), &program.pointSize, 0.3, 3);
        }
    }
    void DrawContent() 
    {
        BeginMode2D(camera);
        program.RenderPass();
        if (drawInterface) program.InterfacePass();
        EndMode2D();
    }

private:
    Ray2Scene scene;
    Ray2Program program;
    Camera2D camera = {};

    float numRaysFact = 0.5;
    float focusFact = 1.0;
    bool hyperbolic = false;
    int shape = 5;
    int currentShape = 5;
    Vector3 color = ColorToHSV({ 0, 255, 0, 255 });
};

#endif