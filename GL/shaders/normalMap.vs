#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aNormal;
layout(location = 2)in vec2 aTexcoord;
layout(location = 3)in vec3 aTangent;
layout(location = 4)in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out VS_OUT
{
	vec2 texcoord;
	vec3 lightDir;
	vec3 viewDir;
}vs_out;

void main()
{
	gl_Position = projection * view * model * aPos;
	vec3 t = normalize(aTangent);
	vec3 b = normalize(aBitangent);
	vec3 n = normalize(aNormal);
	mat3 TBN = mat3(t, b, n);
	vec3 fragPos = mat3(model) * aPos;
	vs_out.lightDir = lightPos - fragPos;
	vs_out.viewDir = viewDir - viewPos;
	vs_out.texcoord = aTexcoord;
}