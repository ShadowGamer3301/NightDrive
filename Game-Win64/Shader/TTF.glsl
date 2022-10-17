//Transform Texture Fragment
#version 410 core

in vec2 UV;
out vec3 color;

uniform sampler2D TexSmp;

void main() {
	color = texture(TexSmp, UV).rgb;
}