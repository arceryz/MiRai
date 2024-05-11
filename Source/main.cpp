#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "Ray2.h"
#include "Ray3.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WORKGROUP_SIZE 1024
#define MAX_POLYGON_POINTS 100

int main()
{
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(800, 800, "Mirai R");
    SetConfigFlags(FLAG_VSYNC_HINT);
    Ray2 ray2;
    Ray3 ray3;
    bool mode3D = true;
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK); 

        if (mode3D) ray3.Draw();
        else ray2.Draw();

        if (GuiButton({ 10, 770, 40, 20 }, TextFormat("%s", mode3D? "3D": "2D"))) mode3D = !mode3D;
        EndDrawing();
    }
}