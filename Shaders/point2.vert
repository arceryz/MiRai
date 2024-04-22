#version 430

// This is the vertex position of the base particle!
// This is the vertex attribute set in the code, index 0.
layout (location=0) in vec2 vertexPosition;

struct LineSegment {
    vec2 u;
    vec2 v;
};

// Two buffers are here.
// The ray ssbo is packed per bounce, ie numPoints * numBounces.
layout(std430, binding=0) buffer lineBuf {  LineSegment segments[]; };
layout(std430, binding=1) buffer rayBuf { float distances[]; };

layout(location=0) uniform int numPoints;
layout(location=1) uniform int numBounces;
layout(location=2) uniform vec2 origin;

// We will only output color.
out vec4 fragColor;

const float PI = 3.14159;

void main()
{
    float dist = distances[gl_InstanceID];
    float prog = float(gl_InstanceID) / numPoints;

    // Ray originate from (0, 0).
    vec2 ray;
    ray.x = cos(2*PI*prog);
    ray.y = sin(2*PI*prog);
    ray *= dist;

    vec2 pos = vertexPosition;
    pos *= 0.01;
    pos += ray;

    gl_Position = vec4(pos.xy, 0, 1);
    fragColor = vec4(0, 1, 0, 1);
}