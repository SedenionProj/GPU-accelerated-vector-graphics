#version 330

layout (location = 0) in vec2 aPos;

uniform mat4 projection;

void main(){
	gl_Position = vec4(aPos,0,1);
}

