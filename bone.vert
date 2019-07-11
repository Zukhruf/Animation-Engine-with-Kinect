#version 330 core
in vec3 linePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(linePos, 1.0f);
}