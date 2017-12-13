#version 330 core

layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aNormal;
layout(location = 2)in vec2 aTexcoord;
layout(location = 3)in vec3 aTangent;
layout(location = 4)in vec3 aBitangent;

out VS_OUT
{
	vec3 normal;
	vec2 texcoord;
	vec3 lightDir;
	vec3 viewDir;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vec3 n = normalize(mat3(model) * aNormal);
	vec3 t = normalize(mat3(model) * aTangent);
	t = normalize(t - dot(t, n) * n);
	//vec3 b = normalize(mat3(model) * aBitangent);
	vec3 b = cross(n, t);
	mat3 TBN = transpose(mat3(t, b, n));

	vec3 fragPos = mat3(model) * aPos;
	vs_out.normal = TBN * aNormal;
	vs_out.lightDir = TBN * (lightPos - fragPos);
	vs_out.viewDir = TBN * (viewPos - fragPos);
	vs_out.texcoord = aTexcoord;
}