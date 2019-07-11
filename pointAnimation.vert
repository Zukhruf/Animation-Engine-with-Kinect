#version 330 core
in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 positionA;
uniform vec4 positionB;
uniform float lerpValue;
uniform float time;

void main(){
	
	vec4 lerp = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float t = abs(sin(time));
	
	lerp = mix(positionA.xy, positionB.xy, lerpValue);
	
	gl_Position = projection * view * model * lerp;
	gl_PointSize = 8.0f;
}