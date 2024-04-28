#include "TriangleNet.h"
#include <raymath.h>

Polygon::Polygon()
{

}
void Polygon::ComputeCenter()
{
    Vector3 sum = {};
    for (Vector3 v: vertices) {
        sum = Vector3Add(sum, v);
    }
    center = Vector3Scale(sum, 1.0/vertices.size());
}
void Polygon::Draw(Color color, Color normalColor, float faceScale, int numEdges)
{
    // This matrix centers to origin, scales and then centers back.
    // A localized scaling around the center of the polygon.
    Matrix transform = MatrixTranslate(-center.x, -center.y, -center.z);
    transform = MatrixMultiply(transform, MatrixScale(faceScale, faceScale, faceScale));
    transform = MatrixMultiply(transform, MatrixTranslate(center.x, center.y, center.z));

    if (numEdges < 0) {
        numEdges = vertices.size();
    }
    // if numEdges==0 then we draw a single vert.
    for (int i = 0; i <= numEdges && i < vertices.size(); i++) {
        DrawSphere(Vector3Transform(vertices[i], transform), 0.02f*faceScale, color);
    }
    // Draw edges stopped at numEdges.
    for (int i = 0; i < numEdges; i++) {
        Vector3 v1 = Vector3Transform(vertices[i], transform);
        Vector3 v2 = Vector3Transform(vertices[(i+1)%vertices.size()], transform);
        DrawLine3D(v1, v2, color);
    }
    Vector3 worldNormal = Vector3Add(center, Vector3Scale(normal, 0.3*faceScale));
    DrawLine3D(center, worldNormal, normalColor);
    DrawSphere(center, 0.02f*faceScale, normalColor);
}

TriangleNet::TriangleNet()
{

}
void TriangleNet::Clear()
{
    vertices.clear();
    vertexToIndex.clear();
    indices.clear();
    indexToNeighbors.clear();
}
void TriangleNet::AddTriangles(vector<Vector3> triangles)
{
    // Iterate every triangle.
    for (int i = 0; i < triangles.size(); i += 3) {
        for (int j = 0; j < 3; j++) { 
            Vector3 vertex = triangles[i+j];
            string key = GetVectorKey(vertex);
            // Add a new vertex if this key is not present.
            if (vertexToIndex.find(key) == vertexToIndex.end()) {
                vertices.push_back(vertex);
                vertexToIndex[key] = vertices.size()-1;
            } 
            indices.push_back(vertexToIndex[key]);
        }

        // Count the number of times each edge is added.
        for (int j = indices.size()-3; j < indices.size(); j++) {
            for (int k = indices.size()-3; k < indices.size(); k++) {
                if (j == k) continue;
                indexToNeighbors[indices[j]][indices[k]] += 1;
            }
        }
    }
}
bool TriangleNet::IsEdgeInternal(int u, int v)
{
    return indexToNeighbors[u][v] != 1;
}
bool TriangleNet::IsVertexInternal(int u)
{
    for (auto kv: indexToNeighbors[u]) {
        if (kv.second < 2)
            return false;
    }
    return true;
}
string TriangleNet::GetVectorKey(Vector3 vec)
{
    return string(TextFormat("%3.3f,%3.3f,%3.3f", vec.x, vec.y, vec.z));
}
void TriangleNet::Draw(Color external, Color internal)
{
    for (int i = 0; i < indices.size(); i += 3) {
        for (int j = 0; j < 3; j++) {
            int i1 = indices[i+j];
            int i2 = indices[i+(j+1)%3];
            Vector3 v1 = vertices[i1];
            Vector3 v2 = vertices[i2];
            DrawLine3D(v1, v2, IsEdgeInternal(i1, i2) ? internal: external);
        }
    }
    for (int i = 0; i < vertices.size(); i++) {
        DrawSphere(vertices[i], 3, IsVertexInternal(i) ? internal: external);
    }
}
void TriangleNet::DrawLabels(Camera3D camera, float size, Color color)
{
    for (int i = 0; i < vertices.size(); i++) {
        Vector3 pos = vertices[i];
        const char *text = TextFormat("%d\n", i);
        DrawText(text, pos.x, pos.y, size, color);
    }
    // Draw all edge counters.
    for (auto kv1: indexToNeighbors) {
        for (auto kv2: kv1.second) {
            if (kv2.second == 0) continue;
            Vector3 pos1 = vertices[kv1.first];
            Vector3 pos2 = vertices[kv2.first];
            Vector3 dir = Vector3Scale(Vector3Subtract(pos2, pos1), 0.3f);
            Vector2 textPos = GetWorldToScreen(Vector3Add(pos1, dir), camera);
            DrawText(TextFormat("%d", kv2.second), textPos.x, textPos.y, size*0.5, color);
        }
    }
}
vector<Polygon> TriangleNet::GetPolygons()
{
    // Find starting vertex.
    int currentVertex = 0;
    for (int i=0; i < vertices.size(); i++) {
        if (!IsVertexInternal(i)) {
            currentVertex = i;
            break;
        }
    }
    
    // Now traverse all verts starting from this one,
    // ignoring those we have already seen.
    unordered_map<int, bool> seen;
    vector<Polygon> list;
    
    for (int i = 0; i < vertices.size(); i++){ 
        // Iterate all vertices to ensure we get all islands.
        // For each new vertex we create a polygon.
        if (seen[i]) continue;
        Polygon poly;

        for (int j = 0; j < vertices.size(); j++) {
            seen[currentVertex] = true;
            poly.vertices.push_back(vertices[currentVertex]);

            int nextVertex = -1;
            for (auto kv: indexToNeighbors[currentVertex]) {
                if (kv.second == 1 && !seen[kv.first]) {
                    nextVertex = kv.first;
                    break;
                }
            }
            if (nextVertex == -1) {
                break;
            }
            currentVertex = nextVertex;
        }

        poly.ComputeCenter();
        list.push_back(poly);
    }

    return list;
}