#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "Ray2Scene.h"
#include "Ray2Program.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WORKGROUP_SIZE 1024
#define MAX_POLYGON_POINTS 100

int main()
{
    InitWindow(800, 800, "Mirai R");
    //SetTargetFPS(144);

    Ray2Scene scene;
    scene.GenerateRegularPolygon(5);

    Ray2Program ray2(scene);
    ray2.numRays = 1024;
    ray2.numBounces = 4;

    float numRaysFact = 0.5;
    float focusFact = 1.0;
    bool hyperbolic = false;
    int shape = 5;
    int currentShape = 5;
    Vector3 color = { 1.0, 1.0, 1.0 };

    Camera2D camera = {};
    camera.offset = { 400, 400 };
    camera.target = { 400, 400 };
    camera.zoom = 1.0;

    while(!WindowShouldClose()) {
        ray2.numRays = (int)(pow(numRaysFact, 8) * MAX_RAYS);
        ray2.color = ColorFromHSV(color.x, color.y, color.z);
        ray2.arcFocus = (hyperbolic ? -1 : 1) * (1+pow(focusFact, 5) * (ARC_FOCUS_INF-1));

        BeginDrawing();
        ClearBackground(BLACK); 

        BeginMode2D(camera);
        ray2.ComputePass();
        ray2.RenderPass();
        EndMode2D();

        DrawText(TextFormat("N=%d", ray2.numRays), 10, 30, 20, DARKGRAY);
        DrawFPS(10, 10);

        float ox = 170;
        float oy = 15;
        GuiSlider({ ox, oy, 200, 10 }, "Rays%", TextFormat("%3.1f", 100*numRaysFact), &numRaysFact, 0, 1);
        GuiSlider({ ox, oy+20, 200, 10 }, "Focus", TextFormat("%.2f", ray2.arcFocus), &focusFact, 0, 1);
        GuiSlider({ ox+270, oy, 100, 10 }, "Zoom", TextFormat("%.1f", camera.zoom), &camera.zoom, 0.1, 2);
        GuiCheckBox({ ox+240, oy+15, 20, 20 }, "Hyperbolic", &hyperbolic);
        GuiSpinner({ ox, oy+40, 80, 15 }, "Num Bounces ", &ray2.numBounces, 1, MAX_BOUNCES, false);
        GuiSpinner({ ox+130, oy+40, 80, 15 }, "Shape ", &shape, 3, 10, false);
    
        ox = 660;
        oy = 13;
        GuiColorPickerHSV({ ox, oy, 100, 70 }, "Color", &color);
        GuiSlider({ ox, oy+80, 100, 10 }, "Falloff", TextFormat("%.2f", ray2.falloff), &ray2.falloff, 0.001, 1);
        GuiSlider({ ox, oy+100, 100, 10 }, "Point Size", TextFormat("%.2f", ray2.pointSize), &ray2.pointSize, 0.3, 3);

        if (currentShape != shape) {
            currentShape = shape;
            scene.GenerateRegularPolygon(shape);
        }
        if (IsKeyPressed(KEY_R)) {
            scene.GenerateRandomCirclePolygon(5, 20);
        }

        EndDrawing();
    }
}