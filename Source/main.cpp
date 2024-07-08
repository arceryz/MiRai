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

const string modelPath = "./Models";
const string shaderPath = "./Shaders";
Color clearColor = BLACK;
Color mainEdgeColor = { 0, 255, 0, 255 };
bool focusMode = false;
RenderTexture2D resultTexture;
  
int main()
{
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(144);
    InitWindow(800, 800, "Mirai R");
    SetExitKey(999999999);

    // Error handling. Check required directories.
    string error = "";
    if (!DirectoryExists(shaderPath.c_str())) error = string("Could not find shader directory ") + shaderPath;
    if (!DirectoryExists(modelPath.c_str())) error = string("Could not find model directory ") + modelPath;
    if (error.length() > 0) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(WHITE);
            if ((int)(GetTime()*2.0) % 2 == 0) DrawText(error.c_str(), 10, 10, 20, RED);
            EndDrawing();
            continue;
        }
        return 0;
    }
    
    printf("Starting up MiRai.\n");
    Ray2 ray2;
    Ray3 ray3;
    bool mode3D = true;
    bool setColor = false;
    bool setMainColor = false;
    bool exportMode = false;
    bool exportEditingText = false;
    char exportPath[256] = {};
    resultTexture = LoadRenderTexture(800, 800);
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(clearColor); 

        // Step 1. Perform Render to texture/Compute shaders.
        if (!exportMode) {
            if (mode3D) ray3.RenderUpdate();
            else ray2.RenderUpdate();
            if (IsKeyPressed(KEY_M)) {
                mainEdgeColor = { 10, 10, 10, 255 };
                clearColor = WHITE;
                ray3.program.innerClearColor = WHITE;
                ray3.program.edgeThickness = 2.5;
                ray3.program.dynamicResolution = false;
                ray3.program.resolutionPercent = 1.0;
                ray2.program.falloff = 0;
                ray2.numRaysFact = 0.7;
            }
        }

        // Step 2. Render to output texture.
        BeginTextureMode(resultTexture);
        ClearBackground({ 0, 0, 0, 0 });
        
        BeginBlendMode(BLEND_CUSTOM_SEPARATE);
        if (mode3D) {
            rlSetBlendFactorsSeparate(
                RL_SRC_ALPHA, RL_ONE_MINUS_SRC_ALPHA, 
                RL_ONE, RL_ONE_MINUS_SRC_ALPHA, 
                RL_FUNC_ADD, RL_FUNC_ADD);
        }
        else {
            rlSetBlendFactorsSeparate(RL_ONE, RL_ONE, RL_ONE, RL_ONE, RL_MAX, RL_MAX);
        }
        
        if (mode3D) ray3.DrawContent();
        else ray2.DrawContent();
    
        EndBlendMode();
        EndTextureMode();

        // Step 3. Render to screen.
        DrawTexturePro(resultTexture.texture, { 0, 0, 800, -800 }, { 0, 0, 800, 800 }, {}, 0, WHITE);
        if (!exportMode) {
            if (mode3D) ray3.DrawGUI(); 
            else ray2.DrawGUI();
        }

        if (GuiButton({ 60, 770, 60, 20 }, "Save As")) exportMode = !exportMode; 
        if (exportMode) {
            GuiDrawText("Specify the path to save rendered image", { 200, 680, 400, 20 }, TEXT_ALIGN_CENTER, WHITE);
            if (GuiTextBox({ 200, 700, 400, 20}, exportPath, 256, exportEditingText)) exportEditingText = !exportEditingText;
            if (GuiButton({ 350, 730, 100, 20 }, "Confirm")) {
                 Image im = LoadImageFromTexture(resultTexture.texture);
                 ImageFlipVertical(&im);
                 ExportImage(im, exportPath);
                 UnloadImage(im);
                 exportMode = false;
                 printf("Image exported to \"%s\"\n", exportPath);
            }
        }
        if (GuiButton({ 550, 770, 110, 20 }, "Main Edge Color")) setMainColor = !setMainColor;
        if (setMainColor) GuiColorPicker({ 550, 650, 85, 100 }, "Color", &mainEdgeColor);
        if (GuiButton({ 670, 770, 110, 20 }, "Background Color")) setColor = !setColor;
        if (setColor) GuiColorPicker({ 670, 650, 85, 100 }, "Color", &clearColor);
        if (GuiButton({ 10, 770, 40, 20 }, TextFormat("%s", mode3D? "3D": "2D"))) mode3D = !mode3D;
        if (IsKeyPressed(KEY_H) || GuiButton({ 130, 770, 60, 20 }, TextFormat("%s (H)", focusMode ? "Normal": "Focus"))) focusMode = !focusMode;
        EndDrawing();
    }
}