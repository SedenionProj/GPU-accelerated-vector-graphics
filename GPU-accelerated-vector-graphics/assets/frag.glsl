#version 460

out vec4 fragColor;
uniform vec2  u_resolution;
void main()
{
    float grad = 1.5*gl_FragCoord.x/u_resolution.x;
    fragColor = vec4(1-grad,0,grad,1);
}