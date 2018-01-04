#version 330 core

layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aNormal;
layout(location=2)in vec2 aTexcoord;

out VS_OUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texcoord;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.worldPos = mat3(model) * aPos;
	vs_out.normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.texcoord = aTexcoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}