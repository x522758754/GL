#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aNormal;
layout(location = 2)in vec2 aTexcoord;
layout(location = 3)in vec3 aTangent;
layout(location = 4)in vec3 aBitangent;

out VS_OUT
{
	vec2 texcoord;
	vec3 fragPos;
	vec3 lightDir;
	vec3 viewDir;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform vec3 lightPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1);
	vec3 T = normalize(mat3(model) * aTangent);
	vec3 N = normalize(transpose(mat3(model)) * aNormal);
	//矫正T，防止有不垂直的情况
	T = normalize(T - N * dot(T, N));
	vec3 B = cross(T, N);
	mat3 TBN = mat3(T, B, N);
	TBN = transpose(TBN);
	vec3 fragPos = mat3(model) * aPos;
	vec3 lightDir = lightPos - fragPos;
	vec3 viewDir = lightPos - fragPos;
	vs_out.texcoord = aTexcoord;
	vs_out.fragPos = TBN * fragPos;
	vs_out.lightDir = TBN * lightDir;
	vs_out.viewDir = TBN * viewDir;
}