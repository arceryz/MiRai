#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "Polygon.h"

#define WORKGROUP_SIZE 1024
#define MAX_POLYGON_POINTS 100

int main()
{
    InitWindow(800, 800, "Mirai R");
    //SetTargetFPS(144);

    Polygon polygon;
    polygon.Generate(5);

    // Specify root parameters.
    int numWorkgroups = 100;
    int numBounces = 16;

    // Load shader.
    int ray2program; {
        char *code = LoadFileText("Shaders/ray2.comp");
        int shader = rlCompileShader(code, RL_COMPUTE_SHADER);
        ray2program = rlLoadComputeShaderProgram(shader);
        UnloadFileText(code);   
    }
    Shader pointProgram = LoadShader("Shaders/point2.vert", "Shaders/point2.frag");

    // Setup the SSBO's for the points and for the line segments.
    int lineBuf = rlLoadShaderBuffer(MAX_POLYGON_POINTS*sizeof(Vector2), polygon.points.data(),
        RL_DYNAMIC_READ);
    int dataSize = sizeof(Vector2)*numWorkgroups*WORKGROUP_SIZE*numBounces;
    Vector2 *rayData = (Vector2*)RL_MALLOC(dataSize);
    int rayBuf = rlLoadShaderBuffer(dataSize, rayData, RL_DYNAMIC_COPY);

    int pointVao = rlLoadVertexArray(); {
        rlEnableVertexArray(pointVao);

        // Our point mesh. Just a triangle.
        Vector2 vertices[] = {
            { -0.86, -0.5 },
            { 0.86, -0.5 },
            { 0.0f,  1.0f }
        };

        // Configure the vertex array with a single attribute of vec2.
        // This is the input to the vertex shader.
        rlEnableVertexAttribute(0);
        rlLoadVertexBuffer(vertices, sizeof(vertices),  false);
        rlSetVertexAttribute(0, 2, RL_FLOAT, false, 0, 0); 
        rlDisableVertexArray();
    }


    while(!WindowShouldClose()) {
        int numPoints = numWorkgroups*WORKGROUP_SIZE;
        int numPolygonPoints = polygon.points.size();

        ClearBackground(BLACK);
        BeginDrawing();

        { // Compute Pass.
            rlEnableShader(ray2program);

            // Set our parameters. The indices are set in the shader.
            rlSetUniform(0, &numPoints, SHADER_UNIFORM_INT, 1);
            rlSetUniform(1, &numBounces, SHADER_UNIFORM_INT, 1);
            rlSetUniform(2, &numPolygonPoints, SHADER_UNIFORM_INT, 1);

            rlBindShaderBuffer(lineBuf, 0);
            rlBindShaderBuffer(rayBuf, 1);

            // Each workgroup has size 1024.
            rlComputeShaderDispatch(numWorkgroups, 1, 1);
            rlDisableShader();
        }

        BeginDrawing();
        ClearBackground(BLACK); 

        { // Render Pass.
            rlEnableShader(pointProgram.id);

            rlSetUniform(0, &numPoints, SHADER_UNIFORM_INT, 1);
            rlSetUniform(1, &numBounces, SHADER_UNIFORM_INT, 1);
            rlBindShaderBuffer(lineBuf, 0);
            rlBindShaderBuffer(rayBuf, 1);

            // Draw the particles. Instancing will duplicate the vertices.
            rlEnableVertexArray(pointVao);
            rlDrawVertexArrayInstanced(0, 3, WORKGROUP_SIZE*numWorkgroups*numBounces);
            rlDisableVertexArray(); 
            rlDisableShader();

            // for (int i = 0; i < polygon.segments.size(); i++) {
            //     Vector2 u = polygon.segments[i].u;
            //     Vector2 v = polygon.segments[i].v;
            //     u = Vector2Add({ 400, 400 }, Vector2Scale(u, 100));
            //     v = Vector2Add({ 400, 400 }, Vector2Scale(v, 100));

            //     DrawLineV(u, v, i%2==0 ? RED : BLUE);
            // }
            DrawText(TextFormat("N=%d", numPoints), 10, 30, 20, DARKGRAY);
            DrawFPS(10, 10);
        }

        bool changed = false;
        if (IsKeyPressed(KEY_O)) {
            changed = true;
            polygon.Generate(max(3, (int)polygon.points.size()-1));
        }
        if (IsKeyPressed(KEY_P)) {
            changed = true;
            polygon.Generate(polygon.points.size()+1);
        }
        if (changed) {
            rlUpdateShaderBuffer(lineBuf, polygon.points.data(), polygon.points.size()*sizeof(Vector2), 0);
        }

        EndDrawing();
    }
}