#include "Polygon.h"
#include <raymath.h>

string Polygon::VectorKey(Vector3 vec)
{
    return string(TextFormat("%.5f:%.5f:%.5f", vec.x, vec.y, vec.z));
}
vector<Polygon> Polygon::FromTriangles(vector<Vector3> triangles)
{
    // Determine edge counters.
    unordered_map<string, unordered_map<string, int>> edgeCounters;
    unordered_map<string, Vector3> keyVertices;
    for (int i=0; i < triangles.size(); i+=3) {
        for (int j=0; j < 3; j++) {
            string k1 = VectorKey(triangles[i+j]);
            string k2 = VectorKey(triangles[i+(j+1)%3]);
            edgeCounters[k1][k2]++;
            edgeCounters[k2][k1]++;
            keyVertices[k1] = triangles[i+j];
        }
    }

    unordered_map<string, bool> seen;
    vector<Polygon> polygons;
    
    // Iterate all vertices and form polygons.
    for (int i = 0; i < triangles.size(); i++){
        Vector3 head = triangles[i];
        string key = VectorKey(head);
        if (seen[key]) continue;
        Polygon poly;

        for (int j = 0; j < triangles.size(); j++) {
            seen[key] = true;
            poly.vertices.push_back(head);
            bool hasNext = false;
            for (auto kv: edgeCounters[key]) {
                if (kv.second == 1 && !seen[kv.first]) {
                    key = kv.first;
                    head = keyVertices[key];
                    hasNext = true;
                    break;
                }
            }
            if (!hasNext) break;
        }
        if (poly.vertices.size() > 1) {
            poly.ComputeCenter();
            polygons.push_back(poly);
        }
    }

    return polygons;
}

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
void Polygon::Draw(Color color, Color normalColor, float scale, float faceScale, int numEdges)
{
    // This matrix centers to origin, scales and then centers back.
    // A localized scaling around the center of the polygon.
    Matrix transform = MatrixTranslate(-center.x, -center.y, -center.z);
    transform = MatrixMultiply(transform, MatrixScale(faceScale, faceScale, faceScale));
    transform = MatrixMultiply(transform, MatrixTranslate(center.x, center.y, center.z));
    transform = MatrixMultiply(transform, MatrixScale(scale, scale, scale));

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