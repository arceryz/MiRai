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
uniform mat4 mvp;

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
void EdgeCollide(vec3 o, vec3 d, out vec3 color);
void CurvedEdgeCollide(vec3 o, vec3 d, out vec3 color);

void main()
{
    // Compute the necessary ray data in view space.
    // We can't do this in vertex space due to projection.;
    vec2 ndc = 2*gl_FragCoord.xy / 800 - 1;
    vec4 originHom = invMvp*vec4(ndc.xy, -1, 1);
    vec4 dirHom = invMvp*vec4(ndc.xy, 1, 1);
    vec3 dirWorld = dirHom.xyz / dirHom.w;
    vec3 rayOriginStart = originHom.xyz / originHom.w;
    vec3 rayDirStart = normalize(dirWorld-rayOriginStart);

    vec3 rayOrigin = rayOriginStart;
    vec3 rayDir = rayDirStart;

    vec3 col = vec3(0, 0, 0);

    // We iterate every reflection.
    // Finding the first mirror collided with.
    for (int b=0; b < numBounces; b++) { 
        HitInfo minHit;
        minHit.t = -1;
        int mirrorIndex = 0;
        float minEdgeProj = 0;

        // Iterate every mirror and collide spheres.
        for (int i=0; i < numMirrors; i++) {
            MirrorInfo mirror = mirrors[i];
            vec3 cornerPoint = vertices[mirror.offset].xyz;

            // 1. Hit with the correct half of the sphere.
            // Spherical=Inside (outside=false)
            vec3 cc = mirror.center.xyz + sphereFocus * mirror.normal.xyz;
            float rsq = DistanceSq(cornerPoint, cc);
            HitInfo hit = RaySphereHit(rayOrigin, rayDir, cc, rsq, false);

            // 2. Validate if outside.
            // Checking if normal dot product is positive or negative.
            // Negative means OUTSIDE since normals point inward.
            vec3 centertohit = normalize(hit.point - mirror.center.xyz);
            bool isOutside = dot(centertohit, mirror.normal.xyz) <= OUTSIDE_MARGIN;

            // 3. Validate if within edge bounds.
            // Iterate each edge and perform circularity check.
            float edgeProj = -1;
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
                }
            }

            bool validity = hit.t > STEP_MIN && isOutside && inBounds;
            bool optimality = minHit.t < 0 || hit.t < minHit.t;
            if (validity && optimality) {
                minHit = hit;
                mirrorIndex = i;
                minEdgeProj = edgeProj;
            }
        }

        if (minHit.t > 0 && minEdgeProj < edgeThickness) {
            col = vec3(0, (1-0.5*minEdgeProj/edgeThickness)*pow(0.85, float(b)), 0);
            break;
        }
        if (minHit.t < 0) break;

        // Reflect.
        rayOrigin = minHit.point;
        rayDir = reflect(rayDir, minHit.normal);
    }
    CurvedEdgeCollide(rayOriginStart, rayDirStart, col);
    finalColor = vec4(col, 1);
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

void EdgeCollide(vec3 o, vec3 d, out vec3 color)
{
    // We draw the outside seperately from mirror reflections.
    // For this we intersect with the spheres but add a validity test
    // that the reflection point must be within the so-called edge-planes.
    for (int i=0; i < numMirrors; i++) {
        MirrorInfo mirror = mirrors[i];
        vec3 cornerPoint = vertices[mirror.offset].xyz;

        HitInfo hit = RayPlaneHit(o, d, mirror.normal.xyz, cornerPoint);
        if (hit.t < 0) continue;

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

void CurvedEdgeCollide(vec3 o, vec3 d, out vec3 color)
{
    // We draw the outside seperately from mirror reflections.
    // For this we intersect with the spheres but add a validity test
    // that the reflection point must be within the so-called edge-planes.
    float closestProj = 0;
    float closestHit = -1;

    for (int i=0; i < numMirrors; i++) {
        MirrorInfo mirror = mirrors[i];
        vec3 cornerPoint = vertices[mirror.offset].xyz;

        // For hyperbolic or non-hyperbolic we must consider either
        // the collisions inside or outside the shape.
        vec3 cc = mirror.center.xyz + sphereFocus*mirror.normal.xyz;
        float rsq = DistanceSq(cc, cornerPoint);
        HitInfo hit = RaySphereHit(o, d, cc, rsq, false);
        if (hit.t <= 0) continue;

        bool isCorrect1 = dot(normalize(hit.point - mirror.center.xyz), mirror.normal.xyz) <= OUTSIDE_MARGIN;
        bool isCorrect2 = dot(normalize(o + d*hit.t2 - mirror.center.xyz), mirror.normal.xyz) <= OUTSIDE_MARGIN;
        float t = (isCorrect1 && isCorrect2) ? min(hit.t, hit.t2): (isCorrect1 ? hit.t: hit.t2);
        vec3 point = o+t*d;

        float edgeProj = -1;
        bool isInside = true;
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
            float proj = dot(edgeNormal, point-u);
            if (proj < 0) {
                isInside = false;
                break;
            }
            else if (proj < edgeProj || edgeProj < 0) {
                edgeProj = proj;
            }
        }  

        // We color if close and valid.
        if (edgeProj < edgeThickness && (closestHit < 0 || t < closestHit) && isInside) {
            closestProj = edgeProj;
            closestHit = t;
        }
    }

    if (closestHit > 0) {
        color = vec3(0, 1-closestProj/edgeThickness, 0);
    }
}