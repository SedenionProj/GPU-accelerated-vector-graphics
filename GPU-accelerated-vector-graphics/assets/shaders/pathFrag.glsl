#version 460

out vec4 fragColor;
uniform vec2  u_resolution;

in vec2 texCoord;

void main()
{

    fragColor = vec4(texCoord,1,1);
}