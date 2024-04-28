#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

out vec3 viewRay;

void main()
{
    // Compute clip coordinates and turn these to NDC coordinates.
    // The NDC coordinates range from 0 to 1.
    vec4 clip = mvp*vec4(vertexPosition, 1.0);
    vec3 ndc = clip.xyz / clip.w;
    viewRay = vec3((ndc.x+1)/2, (ndc.y+1)/2, 0);

    gl_Position = clip;
}