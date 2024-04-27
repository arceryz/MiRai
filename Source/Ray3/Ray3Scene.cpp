#include "Ray3Scene.h"
#include <raymath.h>
#include <unordered_map>
#include <string>

void Face::ComputeCenter()
{
    Vector3 sum = {};
    for (Vector3 v: vertices) {
        sum = Vector3Add(sum, v);
    }
    center = Vector3Scale(sum, 1.0/vertices.size());
}

Ray3Scene::Ray3Scene()
{

}
void Ray3Scene::AddModel(Model model)
{
    // We must take the faces of the model and turn them into polygons.
    // To test this we can add a draw function.
    // Face f;
    // f.vertices.push_back({ 0, 0, 0 });
    // f.vertices.push_back({ 1, 0, 0 });
    // f.vertices.push_back({ 1, 1, 0 });
    // f.vertices.push_back({ 0, 1, 0 });
    // f.normal = { 0, 0, 1 };
    // f.ComputeCenter();
    // faces.push_back(f);

    for (int i = 0; i < model.meshCount; i++) {
        AddMesh(model.meshes[i]);
    }
}
void Ray3Scene::AddMesh(Mesh mesh)
{
    if (mesh.indices == NULL) {
        printf("Ray3Scene mesh require indices\n");
        return;
    }
    printf("Indices found!\n");

    // This map partitions all triangles by colinearity.
    // Using the inner product <n, x> = CONST and normal as string keys.
    unordered_map<string, vector<Vector3*>> colinearTriangles;

    // Interpret as Vector3 array.
    Vector3 *vertices = (Vector3*)mesh.vertices;
    Vector3 *normals = (Vector3*)mesh.normals;

    for (int i = 0; i < mesh.triangleCount; i++) {
        // Indexed vertices.
        int index = mesh.indices[3*i];

        // Every third vertex is a triangle.
        Vector3 *triangle = &vertices[index];
        Vector3 normal = normals[index];
        float dotp = Vector3DotProduct(normal, triangle[0]);

        string key = string(TextFormat("%1.3f:%1.3f:%1.3f:%.3f",
            normal.x, normal.y, normal.z, dotp));
        
        colinearTriangles[key].push_back(triangle);
        printf("Plane Key = %s\n", key.c_str());
    }

    // Iterate all keys for triangles! :)
    for (auto kv: colinearTriangles) {
        vector<Vector3*> &triangles = kv.second;
        printf("Key %s has %d triangles!\n", kv.first.c_str(), triangles.size());
    }
}
void Ray3Scene::DebugDrawMesh(Mesh mesh)
{
    Vector3 *vertices = (Vector3*)mesh.vertices;
    Vector3 *normals = (Vector3*)mesh.normals;

    for (int i = 0; i < mesh.triangleCount*3; i++) {
        Vector3 v = vertices[mesh.indices[i]];
        Vector3 n = Vector3Add(v, Vector3Scale(normals[mesh.indices[i]], 0.3));

        DrawSphere(v, 0.05, BLUE);
        DrawLine3D(v, n, GREEN);
    }
}
void Ray3Scene::DrawFaces(Color color, Color normalColor)
{
    for (Face face: faces) {
        for (int i = 0; i < face.vertices.size(); i++) {
            Vector3 v1 = face.vertices[i];
            Vector3 v2 = face.vertices[(i+1)%face.vertices.size()];
            DrawLine3D(v1, v2, color);

            Vector3 worldNormal = Vector3Add(face.center, Vector3Scale(face.normal, 0.3));
            DrawLine3D(face.center, worldNormal, normalColor);
        }
    }
}