#version 430

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

void main()
{
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}