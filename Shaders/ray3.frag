#version 430

// Input vertex attributes (from vertex shader)
in vec3 rayVector;
in vec3 rayOrigin;
in vec2 ndcCoord;

// Output fragment color
out vec4 finalColor;

// First buffer for vertices.
// Second buffer for polygon counts.
layout(std430, binding=0) buffer ssbo0 { vec4 vertices[]; };
layout(std430, binding=1) buffer ssbo1 { vec4 normals[]; };
layout(std430, binding=2) buffer ssbo2 { int sizes[]; };

layout(location=0) uniform int numMirrors;

struct HitInfo {
    float t;
    vec3 point;
    vec3 normal;
};
struct LineProjection {
    vec3 projection;
    float utov;
};
HitInfo RaySphereHit(vec3 o, vec3 d, vec3 cc, float rsq, bool convex);
HitInfo RayPlaneHit(vec3 o, vec3 d, vec3 n, vec3 u);
LineProjection PointLineProject(vec3 p, vec3 u, vec3 v);
float DistanceSq(vec3 a, vec3 b);

void main()
{
    vec3 col = vec3(0.1);

    // Normalizing is very important since the vertex shader does
    // not interpolate with unit length!
    vec3 rayDir = normalize(rayVector);

    // Iterate all mirrors and keep track of individual
    // mirror offset indices in the main vertex buffer.
    for (int b = 0; b < 1; b++) {
        int offset = 0;
        int mirrorIndex = 0;
        int mirrorOffset = 0;
        HitInfo mirrorHit;
        mirrorHit.t = -1;

        // Find the nearest plane to intersect with valid normal.
        for (int i = 0; i < numMirrors; i++) {
            int size = sizes[i];
            vec3 normal = normals[i].xyz;
            vec3 surfacePoint = vertices[offset].xyz;
            HitInfo hit = RayPlaneHit(rayOrigin, rayDir, normal, surfacePoint);
            if ((mirrorHit.t < 0 || hit.t < mirrorHit.t) && dot(normal, rayDir) < 0) {
                mirrorHit = hit;
                mirrorIndex = i;
                mirrorOffset = offset;
            }
            offset += size;
        }

        col = vec3(distance(-mirrorHit.normal, mirrorHit.point));

    // for (int j = 0; j < size; j++) {
        //     vec3 u = vertices[index+j].xyz;
        //     vec3 v = vertices[index+(j+1)%size].xyz;
        //     float r = 0.1;
        //     //HitInfo hit = RaySphereHit(rayOrigin, rayDir, u, r*r, true);
        //     LineProjection lp = PointLineProject(planeHit.point, u, v);
        //     bool bounds = 0 <= lp.utov && lp.utov <= 1;
        //     if (length(lp.projection - planeHit.point) < 0.05 && bounds) {
        //         col = vec3(1, 0, 0);
        //     }
    }
    
    finalColor = vec4(col, 1);
}

HitInfo RaySphereHit(vec3 o, vec3 d, vec3 cc, float rsq, bool convex)
{
    // Make the point relative to the circle.
    vec3 rel = o-cc;

    float a = 1;
    float b = 2*dot(rel, d);
    float c = dot(rel, rel) - rsq;
    float D = b*b - 4*a*c;

    // We take an absolute value here, but if the
    // square root ends up negative we set t to zero.
    float sqrtD = sqrt(abs(D));
    float t1 = (-b - sqrtD) / (2*a);
    float t2 = (-b + sqrtD) / (2*a);

    float t = float(convex)*max(t1, t2) + (1-float(convex))*min(t1, t2);

    HitInfo hit;
    hit.point = o + t*d;
    hit.normal = normalize(cc - hit.point);
    hit.t = t * float(D >= 0);
    return hit;
}

HitInfo RayPlaneHit(vec3 o, vec3 d, vec3 n, vec3 u)
{
    float nd = dot(n, d);
    float t = dot(n, u-o) / (nd + float(nd==0)*0.001);

    HitInfo hit;
    hit.point = o + t*d;
    hit.normal = n;
    hit.t = t;

    return hit;
}

LineProjection PointLineProject(vec3 p, vec3 u, vec3 v)
{
    float uv = length(v-u);
    vec3 dir = (v-u) / uv;
    float utov = dot(dir, p-u);
    vec3 proj = u + dir*utov;
    LineProjection lp;
    lp.projection = proj;
    lp.utov = utov / uv;
    return lp;
}

float DistanceSq(vec3 a, vec3 b)
{
    return dot(a-b, a-b);
}