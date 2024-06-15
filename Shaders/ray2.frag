#version 430

in vec4 fragColor;
in vec2 localPos;
out vec4 finalColor;

void main()
{
    // There is only one thing to do.
    finalColor.a = fragColor.a*smoothstep(1, 0, dot(localPos, localPos)/0.4);
    finalColor.rgb = fragColor.rgb * fragColor.a;
}