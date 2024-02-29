#version 330

out vec4 FragColor;

in vec2 col;

void main(){
	if(col.x==1)
		FragColor = vec4(1,0,0,1);
	else if(col.x==2)
		FragColor = vec4(0,1,0,1);
	else
		FragColor = vec4(0,0,1,1);
}