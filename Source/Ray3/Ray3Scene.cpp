#include "Ray3Scene.h"
#include <raymath.h>
#include <unordered_map>
#include <string>

Ray3Scene::Ray3Scene()
{

}

void Ray3Scene::AddMirrorModel(Model model, bool flipNormals)
{
    for (int i = 0; i < model.meshCount; i++) {
        AddMirrorMesh(model.meshes[i], flipNormals);
    }
}

void Ray3Scene::AddMirrorMesh(Mesh mesh, bool flipNormals)
{
    int vertexCount = mesh.triangleCount*3;
    vector<Vector3> vertices;
    vector<Vector3> normals;
    vertices.resize(vertexCount);
    normals.resize(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        int index = mesh.indices != NULL ? mesh.indices[i]: i;
        vertices[i] = ((Vector3*)mesh.vertices)[index];
        normals[i] = ((Vector3*)mesh.normals)[index];
    }

    // This map partitions all triangles by colinearity.
    // Using the inner product <n, x> = CONST and normal as string keys.
    unordered_map<string, vector<int>> triangleSets;
    for (int i = 0; i < vertexCount; i += 3) {
        Vector3 normal = normals[i];
        float dotp = Vector3DotProduct(normal, vertices[i]);
        string key = string(TextFormat("%1.3f:%1.3f:%1.3f:%.3f",
            normal.x, normal.y, normal.z, dotp));

        // Group triangles in triangle sets.
        vector<int> &tris = triangleSets[key];
        tris.push_back(i);
        tris.push_back(i+1);
        tris.push_back(i+2);
    }

    for (auto kv: triangleSets) {
        vector<Vector3> triangles;
        for (int i: kv.second) triangles.push_back(vertices[i]);
        vector<Polygon> polygons = Polygon::FromTriangles(triangles);
        for (Polygon &poly: polygons) {
            poly.normal = Vector3Scale(normals[kv.second[0]], flipNormals ? -1: 1);
        }
        //printf("Subnet consists of %d polygons\n", polygons.size());
        mirrors.insert(mirrors.end(), polygons.begin(), polygons.end());
    }
    printf("Extracted %d mirrors\n", mirrors.size());
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

void Ray3Scene::DrawMirrors(float scale, float faceScale)
{
    vector<Color> colors = {
            RED,
            GREEN,
            BLUE,
            ORANGE,
            MAGENTA,
            GRAY
    };
    DrawSphere({}, 0.01f * scale, WHITE);
    for (int i = 0; i < mirrors.size(); i++) {
        Color color = colors[i%colors.size()];
        mirrors[i].Draw(color, color, scale, faceScale);
    }
}

void Ray3Scene::NormalizeRadius()
{
    float largestDist = -1;
    for (Polygon &p: mirrors) {
        for (Vector3 v: p.vertices) {
            float length = Vector3Length(v);
            if (largestDist < 0 || length > largestDist) {
                largestDist = length;
            }
        }
    }
    for (Polygon &p: mirrors) {
        p.Scale(1.0f / largestDist);
    }
}

void Ray3Scene::Centralize()
{
    Vector3 sum = {};
    for (Polygon &p: mirrors) {
        sum = Vector3Add(sum, p.GetCenter());
    }
    sum = Vector3Scale(sum, 1.0f / mirrors.size());
    for (Polygon &p: mirrors) {
        p.Translate(Vector3Negate(sum));
    }
}

vector<Vector4> Ray3Scene::GetMirrorVerticesPacked()
{
    vector<Vector4> vertices;
    for (Polygon poly: mirrors) {
        for (Vector3 vertex: poly.vertices) {
            // TODO: Use this free data for something cool.
            vertices.push_back({ vertex.x, vertex.y, vertex.z, 0 });
        }
    }
    return vertices;
}

vector<MirrorInfo> Ray3Scene::GetMirrorInfosPacked()
{
    vector<MirrorInfo> infos;
    int offset = 0;
    for (Polygon poly: mirrors) {
        MirrorInfo info = {};
        Vector3 center = poly.GetCenter();
        info.normal = { poly.normal.x, poly.normal.y, poly.normal.z, 0 };
        info.center = { center.x, center.y, center.z, 0 };
        info.vertexOffset = offset;
        info.vertexCount = poly.vertices.size();
        infos.push_back(info);
        offset += info.vertexCount;
    }
    return infos;
}

int Ray3Scene::GetMirrorCount()
{
    return mirrors.size();
}