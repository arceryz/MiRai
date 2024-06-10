#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "Config.h"
#include "Ray2.h"
#include "Ray3.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WORKGROUP_SIZE 1024
#define MAX_POLYGON_POINTS 100

Color clearColor = BLACK;

int main()
{
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(800, 800, "Mirai R");
    SetConfigFlags(FLAG_VSYNC_HINT);
    printf("Starting up MiRai.\n");
    Ray2 ray2;
    Ray3 ray3;
    bool mode3D = true;
    bool setColor = false;
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(clearColor); 

        if (mode3D) ray3.Draw();
        else ray2.Draw();

        if (GuiButton({ 650, 770, 110, 20 }, "Background Color")) setColor = !setColor;
        if (setColor) GuiColorPicker({ 650, 650, 110, 100 }, "Color", &clearColor);
        if (GuiButton({ 10, 770, 40, 20 }, TextFormat("%s", mode3D? "3D": "2D"))) mode3D = !mode3D;
        EndDrawing();
    }
}