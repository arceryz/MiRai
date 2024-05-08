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
uniform mat4 mvp;

const float STEP_MIN = 0.001f;

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
    // Compute the necessary ray data in view space.
    // We can't do this in vertex space due to projection.;
    vec2 ndc = 2*gl_FragCoord.xy / 800 - 1;
    vec4 originHom = invMvp*vec4(ndc.xy, -1, 1);
    vec4 dirHom = invMvp*vec4(ndc.xy, 1, 1);
    vec3 dirWorld = dirHom.xyz / dirHom.w;
    vec3 rayOrigin = originHom.xyz / originHom.w;
    vec3 rayDir = normalize(dirWorld-rayOrigin);

    vec3 col = vec3(0);

    // Iterate all mirrors and keep track of individual
    // mirror offset indices in the main vertex buffer.
    for (int b = 0; b < 1; b++) {
        // We keep track of the closest mirror hit.
        HitInfo closestMirrorHit;
        closestMirrorHit.t = -1;
        MirrorInfo closestMirror;
        float closestEdgeProj = -1;

        // Find the nearest plane to intersect with valid normal.
        for (int i = 0; i < 1; i++) {
            // For spherical collision we must test three things.
            // 1. The ray must collide with the sphere.
            //    We take the second collision and test if it is on the correct side.
            // 2. We test if the collision point is within the bounds of the edge planes.
            MirrorInfo mirror = mirrors[i];
            vec3 normal = mirror.normal.xyz;
            vec3 center = mirror.center.xyz;
            vec3 surfacePoint = vertices[mirror.offset].xyz;

            // First we collide with the sphere.
            vec3 sphereCenter = center + sphereFocus*normal;
            float rsq = DistanceSq(sphereCenter, surfacePoint);
            HitInfo hit = RaySphereHit(rayOrigin, rayDir, sphereCenter, rsq, false);

            // Test if the collision occured at the correct side.
            // For focus > 0, we have spherical space and we collide with the outside.
            // For focus < 0, we have hyperbolic space and we collide with the inside.
            // Hyperbolic space does not require any special tests.
            float planeProj = dot(hit.point-center, normal);

            // Now we compute the closest edge projection.
            // We iterate each edge and project on the normal given by the plane spanned by
            // the outward normal at a point and its edge vector.
            // We face the normal of the plane toward our side, then a projection > 0 is correct. 
            float edgeProj = -1;
            for (int j = 0; j < mirror.vertexCount; j++) {
                vec3 u = vertices[mirror.offset + j].xyz;
                vec3 v = vertices[mirror.offset + (j+1)%mirror.vertexCount].xyz;
                
                // Now the normal is given by the (v-u) x u, since our vertices lie on the sphere.
                // We flip the normal to face the center of the face.
                vec3 edgeNormal = normalize(cross(v-u, u));
                edgeNormal *= sign(dot(edgeNormal, center-u));

                // Now we project with the vector from u to the hit point.
                float proj = dot(edgeNormal, hit.point-u);
                if (proj >= 0 && (proj < edgeProj || edgeProj < 0)) {
                    edgeProj = proj;
                }
            }
            edgeProj = 0.01;
            
            bool validity = planeProj < 0 && hit.t > STEP_MIN;
            bool optimality = closestMirrorHit.t < 0 || hit.t < closestMirrorHit.t;
            
            // HitInfo hit = RayPlaneHit(rayOrigin, rayDir, normal, surfacePoint);
            // bool validity = hit.t > STEP_MIN && dot(rayDir, normal) < 0;
            // bool optimality = closestMirrorHit.t < 0 || hit.t < closestMirrorHit.t;

            if (optimality && validity) {
                closestMirrorHit = hit;
                closestMirror = mirror;
                closestEdgeProj = edgeProj;
            }
        }
        
        // if (closestMirrorHit.t < 0) {
        //     break;
        // }

        if (closestEdgeProj > 0 && closestEdgeProj < 0.05) {
            //col = vec3(0, (1-0.5*closestEdgeProj/edgeThickness)*pow(0.85, float(b)), 0);
            col = vec3(0, 0, 1);
            break;
        }
        // bool hasHit = false;
        // for (int j = 0; j < closestMirror.vertexCount; j++) {
        //     vec3 u = vertices[closestMirror.offset+j].xyz;
        //     vec3 v = vertices[closestMirror.offset+(j+1)%closestMirror.vertexCount].xyz;

        //     // Project point on the line and use distance as color.
        //     LineProjection lp = PointLineProject(closestMirrorHit.point, u, v);
        //     bool bounds = 0 <= lp.utov && lp.utov <= 1;
        //     float ds = DistanceSq(lp.projection, closestMirrorHit.point);
        //     if (ds < edgeThickness && bounds) {
        //         col = vec3(0, (1-0.5*ds/edgeThickness)*pow(0.85, float(b)), 0);
        //         hasHit = true;
        //         break;
        //     }
        // }
        // if (hasHit) break;

        // Dont reflect on backward facing normals, but do allow collision
        // with the edges of that mirror.
        rayOrigin = closestMirrorHit.point;
        rayDir = reflect(rayDir, closestMirrorHit.normal);
    }
    
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