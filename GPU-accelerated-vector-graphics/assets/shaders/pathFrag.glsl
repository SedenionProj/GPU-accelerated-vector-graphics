#version 460

out vec4 fragColor;
uniform vec2 u_resolution;

in vec2 texCoord;

in vec4 color;

void main()
{

    fragColor = color;
}