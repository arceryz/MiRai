#version 430

// This is the vertex position of the base particle!
// This is the vertex attribute set in the code, index 0.
layout (location=0) in vec2 vertexPosition;

// Two buffers are here.
// The ray ssbo is packed per bounce, ie numPoints * numBounces.
layout(std430, binding=0) buffer pointSSBO {  vec2 points[]; };
layout(std430, binding=1) buffer distanceSSBO { float distances[]; };

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
    float dist = distances[gl_InstanceID];
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
    fragColor = vec4(color*fade, 1);
}