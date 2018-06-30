#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;


void main()
{
	FragColor = vec4(0.04, 0.28, 0.26, 1.0);
}