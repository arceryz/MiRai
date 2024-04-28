#version 330

// Input vertex attributes (from vertex shader)
in vec3 viewRay;

// Output fragment color
out vec4 finalColor;

void main()
{
    finalColor = vec4(viewRay.xy, 0, 1);
}