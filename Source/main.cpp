#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "Ray2.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WORKGROUP_SIZE 1024
#define MAX_POLYGON_POINTS 100

int main()
{
    InitWindow(800, 800, "Mirai R");
    Ray2 ray2;
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK); 

        ray2.Draw();

        EndDrawing();
    }
}