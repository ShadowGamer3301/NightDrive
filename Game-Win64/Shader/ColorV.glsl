#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;

out vec3 fCol;

uniform mat4 MVP;

void main() {

	gl_Position = MVP * vec4(vPos, 1);
	fCol = vCol; //Pass color data to fragment shader

}