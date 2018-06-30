#version 330 core
layout(location = 0)in vec3 aPosition;
layout(location = 2)in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * view * vec4(aPosition, 1.0);
	TexCoords = aTexCoords;
}