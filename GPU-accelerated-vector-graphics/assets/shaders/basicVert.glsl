#version 330

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aCol;

uniform mat4 projection;

out vec2 col;

void main(){
	col = aCol;
	gl_Position = vec4(aPos,0,1);
	gl_PointSize = 10.;
}

