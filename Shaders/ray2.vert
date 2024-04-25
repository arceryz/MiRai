#version 430

// This is the vertex position of the base particle!
// This is the vertex attribute set in the code, index 0.
layout (location=0) in vec2 vertexPosition;

// The distance ssbo is packed per bounce, ie numPoints * numBounces.
layout(std430, binding=0) buffer distanceSSBO { vec2 distances[]; };

layout(location=0) uniform int numRays;
layout(location=1) uniform int numBounces;
layout(location=2) uniform vec3 color;
layout(location=3) uniform float falloff;
layout(location=4) uniform float pointSize;

// We will only output color.
out vec4 fragColor;
out vec2 localPos;

const float PI = 3.14159;

void main()
{
    vec2 dvec = distances[gl_InstanceID];
    float dist = dvec.x;
    float dmin = dvec.y;

    int depth = gl_InstanceID % numBounces;
    float prog = float(gl_InstanceID/numBounces) / numRays;

    // Ray originate from (0, 0).
    vec2 ray;
    ray.x = cos(2*PI*prog);
    ray.y = sin(2*PI*prog);
    ray *= 0.1*dist;

    vec2 pos = vertexPosition;
    pos *= 0.01 * pointSize;
    pos += ray;
    
    gl_Position = vec4(pos.xy, 0, 1);
    localPos = vertexPosition;

    float fade = pow(falloff, depth);
    //fade *= 0.3+0.7*float(dmin<0.1);
    fragColor = vec4(color*fade, 1);
}