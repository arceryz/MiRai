#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "Polygon.h"
#include "Ray2Program.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WORKGROUP_SIZE 1024
#define MAX_POLYGON_POINTS 100

int main()
{
    InitWindow(800, 800, "Mirai R");
    //SetTargetFPS(144);

    Polygon polygon;
    polygon.Generate(5);

    Ray2Program ray2;
    ray2.numRays = 1024;
    ray2.numBounces = 4;
    ray2.UpdatePoints(polygon.points.size(), polygon.points.data());

    float numRaysPercent = 0.5;
    int shape = 5;
    Vector3 color = { 1.0, 1.0, 1.0 };

    while(!WindowShouldClose()) {
        ray2.numRays = (int)(pow(numRaysPercent, 8) * MAX_RAYS);
        ray2.color = ColorFromHSV(color.x, color.y, color.z);

        BeginDrawing();
        ClearBackground({}); 

        ray2.ComputePass();
        ray2.RenderPass();

        DrawText(TextFormat("N=%d", ray2.numRays), 10, 30, 20, DARKGRAY);
        DrawFPS(10, 10);

        GuiSpinner({ 200, 40, 80, 15 }, "Num Bounces ", &ray2.numBounces, 1, MAX_BOUNCES, false);
        GuiSpinner({ 330, 40, 80, 15 }, "Shape ", &shape, 3, 10, false);
        GuiSlider({ 200, 20, 200, 10 }, "Rays%", TextFormat("%3.1f", 100*numRaysPercent), &numRaysPercent, 0, 1);
        GuiColorPickerHSV({ 650, 20, 100, 70 }, "Color", &color);
        GuiSlider({ 650, 100, 100, 10 }, "Falloff", TextFormat("%.2f", ray2.falloff), &ray2.falloff, 0.001, 1);
        GuiSlider({ 650, 120, 100, 10 }, "Point Size", TextFormat("%.2f", ray2.pointSize), &ray2.pointSize, 0.3, 5);

        if (polygon.points.size() != shape) {
            polygon.Generate(shape);
            ray2.UpdatePoints(polygon.points.size(), polygon.points.data());
        }
        if (IsKeyPressed(KEY_O)) {
            polygon.Generate(max(3, (int)polygon.points.size()-1));
            ray2.UpdatePoints(polygon.points.size(), polygon.points.data());
        }
        if (IsKeyPressed(KEY_P)) {
            polygon.Generate(polygon.points.size()+1);
            ray2.UpdatePoints(polygon.points.size(), polygon.points.data());
        }

        EndDrawing();
    }
}