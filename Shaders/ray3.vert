#version 430

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// rayDirection = vertex-rayOrigin.
out vec3 rayVector;
out vec3 rayOrigin;
out vec2 ndcCoord;

void main()
{
    // Our model matrix is always identity.
    mat4 invViewProj = inverse(mvp);

    // A scaling.
    vec3 vert = vertexPosition;

    // We only need to determine our ray origin in world space.
    // This is determined by the projection of our camera.
    // Once we have determined the ray origin, the direction is
    // to simply point at the vertex.
    vec4 clip = mvp*vec4(vert, 1.0);
    vec3 ndc = clip.xyz / clip.w;
    ndcCoord = ndc.xy;

    // The origin lies on the near plane. Therefore z=-1.
    // Now project back to get the point on the near plane.
    ndc.z = -1;
    vec4 originHom = invViewProj*vec4(ndc.xyz, 1);
    rayOrigin = originHom.xyz / originHom.w;
    rayVector = vert-rayOrigin;

    gl_Position = clip;
}