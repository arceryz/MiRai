#version 430

// Output fragment color
out vec4 finalColor;

struct MirrorInfo {
    vec4 normal;
    vec4 center;
    int offset;
    int vertexCount;
    float p2, p3;
};

// First buffer for vertices.
// Second buffer for polygon counts.
layout(std430, binding=0) buffer ssbo0 { vec4 vertices[]; };
layout(std430, binding=1) buffer ssbo1 { MirrorInfo mirrors[]; };

layout(location=0) uniform int numMirrors;
layout(location=1) uniform float edgeThickness;
layout(location=2) uniform mat4 invMvp;
layout(location=3) uniform float sphereFocus;
layout(location=4) uniform int numBounces;
layout(location=5) uniform int resolution;
layout(location=6) uniform float falloff;
layout(location=7) uniform int showMarkFlags;
layout(location=8) uniform float markSize;
layout(location=9) uniform vec4 innerClearColor;
layout(location=10) uniform vec3 edgeColor;
layout(location=11) uniform int lowerBounceLimit;
uniform mat4 mvp;

const float FOCUS_INF = 1000.0f;
const float STEP_MIN = 0.001f;
const float OUTSIDE_MARGIN = 0.001f;

struct HitInfo {
    float t, t2;
    vec3 point;
    vec3 normal;
};
struct LineProjection {
    vec3 point;
    float utov;
};
HitInfo RaySphereHit(vec3 o, vec3 d, vec3 cc, float rsq, bool convex);
HitInfo RayPlaneHit(vec3 o, vec3 d, vec3 n, vec3 u);
LineProjection PointLineProject(vec3 p, vec3 u, vec3 v);
float DistanceSq(vec3 a, vec3 b);
void EdgeCollide(vec3 o, vec3 d, inout vec3 color);
void CurvedEdgeCollide(vec3 o, vec3 d, out vec3 color);

void main()
{
    // Compute the necessary ray data in view space.
    // We can't do this in vertex space due to projection.;
    vec2 ndc = 2*gl_FragCoord.xy / resolution - 1;
    vec4 originHom = invMvp*vec4(ndc.xy, -1, 1);
    vec4 dirHom = invMvp*vec4(ndc.xy, 1, 1);
    vec3 dirWorld = dirHom.xyz / dirHom.w;
    vec3 rayOriginStart = originHom.xyz / originHom.w;
    vec3 rayDirStart = normalize(dirWorld-rayOriginStart);

    vec3 rayOrigin = rayOriginStart;
    vec3 rayDir = rayDirStart;

    vec4 col = innerClearColor;
    bool showMark = (showMarkFlags&1) == 1;
    bool showEdgeMark = (showMarkFlags&2) == 2;
    bool showEdges = (showMarkFlags&4) == 4;
    bool hyperbolic = sphereFocus > 0;
    bool straight = abs(sphereFocus) > FOCUS_INF-1.0;
    bool isInBox = true;

    // We iterate every reflection.
    // Finding the first mirror collided with.
    int b;
    for (b=0; b < numBounces; b++) { 
        if (showMark) {
            HitInfo rhit = RaySphereHit(rayOrigin, rayDir, vec3(0, 0, 0), markSize*markSize, true);
            if (rhit.t > 0) {
                col.rgb = vec3(0, 0, 1) * dot(-rhit.normal, rayDir) * pow(1-falloff, float(b));
                col.a = 1.0;
                break;
            }
        }

        HitInfo minHit;
        minHit.t = -1;
        int mirrorIndex = 0;
        int minEdgeIndex = 0;
        float minEdgeProj = 0;

        // Iterate every mirror and collide spheres.
        for (int i=0; i < numMirrors; i++) {
            MirrorInfo mirror = mirrors[i];
            vec3 cornerPoint = vertices[mirror.offset].xyz;

            // First we perform a collision test with either flat planes
            // or with sphere arcs.
            // For sphere arcs we set an additional boolean to check if we're on the
            // correct side of the collision.
            HitInfo hit;
            bool correctSide = true;
            if (straight) {
                hit = RayPlaneHit(rayOrigin, rayDir, mirror.normal.xyz, cornerPoint);
                correctSide = dot(mirror.normal.xyz, rayDir) < 0;
            }
            else {
                vec3 cc = sphereFocus * -mirror.normal.xyz;
                float rsq = DistanceSq(cornerPoint, cc);
                hit = RaySphereHit(rayOrigin, rayDir, cc, rsq, hyperbolic);

                // 2. Validate if outside.
                // Checking if normal dot product is positive or negative.
                // Negative means OUTSIDE since normals point inward.
                vec3 centertohit = normalize(hit.point - mirror.center.xyz);
                correctSide = hyperbolic ? dot(centertohit, mirror.normal.xyz) >= -OUTSIDE_MARGIN: 
                    dot(centertohit, mirror.normal.xyz) <= OUTSIDE_MARGIN;
            }
            if (!correctSide) continue;

            // 3. Validate if within edge bounds.
            // Iterate each edge and perform circularity check.
            float edgeProj = -1;
            int edgeIndex = 0;
            bool inBounds = true;
            for (int j = 0; j < mirror.vertexCount; j++) {
                vec3 u = vertices[mirror.offset + j].xyz;
                vec3 v = vertices[mirror.offset + (j+1)%mirror.vertexCount].xyz;
                
                // Now the normal is given by the (v-u) x u, since our vertices lie on the sphere.
                // We flip the normal to face the center of the face.
                vec3 edgeNormal = normalize(cross(v-u, u));
                edgeNormal *= sign(dot(edgeNormal, mirror.center.xyz-u));

                // Now we project with the vector from u to the hit point.
                // Negative projections indicate we are outside the polygonal region!
                // We ignore this mirror collision then.
                float proj = dot(edgeNormal, hit.point-u);
                if (proj < 0) {
                    inBounds = false;
                    break;
                }
                else if (proj < edgeProj || edgeProj < 0) {
                    edgeProj = proj;
                    edgeIndex = j;
                }
            }

            bool validity = hit.t > STEP_MIN && inBounds;
            bool optimality = minHit.t < 0 || hit.t < minHit.t;
            if (validity && optimality) {
                minHit = hit;
                mirrorIndex = i;
                minEdgeProj = edgeProj;
                minEdgeIndex = edgeIndex;
            }
        }

        // Edge collision.
        if (b >= lowerBounceLimit && minHit.t > 0 && minEdgeProj < edgeThickness && (showEdges || (showEdgeMark && mirrorIndex == 0))) {
            vec3 colors[] = {
                vec3(1, 0, 0),
                edgeColor,
                vec3(0, 0, 1)
            };
            float e = (1-0.5*minEdgeProj/edgeThickness);
            float f = e*pow(1-falloff, float(b));
            col.rgb = mix(innerClearColor.rgb, colors[showEdgeMark ? (mirrorIndex == 0 ? 0 : 1): 1], f);
            col.a = 1.0;
            break;
        }
        if (minHit.t < 0 && b == 0) {
            isInBox = false;
            break;
        }

        // Reflect.
        rayOrigin = minHit.point;
        rayDir = reflect(rayDir, minHit.normal);
    }
    finalColor = col;
    finalColor.a = float(isInBox) * col.a;
}

HitInfo RaySphereHit(vec3 o, vec3 d, vec3 cc, float rsq, bool outside)
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

    // Outside collides only from outside. Inside only after entering.
    float t = float(outside)*min(t1, t2) + (1-float(outside))*max(t1, t2);

    HitInfo hit;
    hit.point = o + t*d;
    // We must also flip normals if we do inside or outside collisions!
    // Outside will evaluate to *1.
    hit.normal = (2*float(outside) - 1)*normalize(hit.point-cc);
    hit.t = t * float(D >= 0);
    hit.t2 = t == t1 ? t2 : t1;
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
    lp.point = proj;
    lp.utov = utov / uv;
    return lp;
}

float DistanceSq(vec3 a, vec3 b)
{
    return dot(a-b, a-b);
}

void EdgeCollide(vec3 o, vec3 d, inout vec3 color)
{
    // We draw the outside seperately from mirror reflections.
    // For this we intersect with the spheres but add a validity test
    // that the reflection point must be within the so-called edge-planes.
    for (int i=0; i < numMirrors; i++) {
        MirrorInfo mirror = mirrors[i];
        vec3 cornerPoint = vertices[mirror.offset].xyz;
        if (dot(mirror.normal.xyz, d) < 0) continue;

        HitInfo hit = RayPlaneHit(o, d, mirror.normal.xyz, cornerPoint);
        if (hit.t < STEP_MIN) continue;

        // STAGE 1: EDGE VISUALISATION.
        bool stop = false;
        for (int j=0; j < mirror.vertexCount; j++) {
            vec3 u = vertices[mirror.offset+j].xyz;
            vec3 v = vertices[mirror.offset+(j+1)%mirror.vertexCount].xyz;

            LineProjection lp = PointLineProject(hit.point, u, v);
            float dist = DistanceSq(lp.point, hit.point);
            float r = edgeThickness;
            if (dist < r*r && 0 <= lp.utov && lp.utov <= 1) {
                color = vec3(0, 0, dist/(r*r));
                stop = true;
            }
        }
        if (stop) break;
    }
}