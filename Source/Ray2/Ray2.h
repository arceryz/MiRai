#ifndef RAY2_H
#define RAY2_H

#include "Ray2Scene.h"
#include "Ray2Program.h"
#include "raygui.h"

// This class contains the Ray2 running/UI code.
class Ray2 
{
public:
    Ray2(): program(scene)
    {
        scene.GenerateRegularPolygon(5);
        program.numRays = 1024;
        program.numBounces = 4;
        camera.offset = { 400, 400 };
        camera.target = { 400, 400 };
        camera.zoom = 1.0;
    };
    void Draw() 
    {
        program.numRays = (int)(pow(numRaysFact, 8) * MAX_RAYS);
        program.color = ColorFromHSV(color.x, color.y, color.z);
        program.arcFocus = (hyperbolic ? -1 : 1) * (pow(focusFact, 5) * (ARC_FOCUS_INF-1));

        BeginMode2D(camera);
        program.ComputePass();
        program.RenderPass();
        EndMode2D();

        DrawText(TextFormat("N=%d", program.numRays), 10, 30, 20, DARKGRAY);
        DrawFPS(10, 10);

        float ox = 170;
        float oy = 15;
        GuiSlider({ ox, oy, 200, 10 }, "Rays%", TextFormat("%3.1f", 100*numRaysFact), &numRaysFact, 0, 1);
        GuiSlider({ ox, oy+20, 200, 10 }, "Focus", TextFormat("%.2f (r=%.3f)", -program.arcFocus, program.GetArcRadius()), &focusFact, 0.001, 1);
        GuiSlider({ ox+270, oy, 100, 10 }, "Zoom", TextFormat("%.1f", camera.zoom), &camera.zoom, 0.1, 2);
        GuiCheckBox({ ox+315, oy+15, 20, 20 }, "Hyperbolic", &hyperbolic);
        GuiSpinner({ ox, oy+40, 80, 15 }, "Num Bounces ", &program.numBounces, 1, MAX_BOUNCES, false);
        GuiSpinner({ ox+130, oy+40, 80, 15 }, "Shape ", &shape, 3, 10, false);
    
        ox = 660;
        oy = 13;
        GuiColorPickerHSV({ ox, oy, 100, 70 }, "Color", &color);
        GuiSlider({ ox, oy+80, 100, 10 }, "Falloff", TextFormat("%.2f", program.falloff), &program.falloff, 0.001, 1);
        GuiSlider({ ox, oy+100, 100, 10 }, "Point Size", TextFormat("%.2f", program.pointSize), &program.pointSize, 0.3, 3);

        if (currentShape != shape) {
            currentShape = shape;
            scene.GenerateRegularPolygon(shape);
        }
        if (IsKeyPressed(KEY_R)) {
            scene.GenerateRandomCirclePolygon(5, 20);
        }
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