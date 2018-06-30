#version 330 core
layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aNormal;
layout(location=2)in vec2 aTexCoords;
layout(location=3)in vec3 aTangent;

out VS_OUT{
	vec3 vertexPos;
	vec3 lightPos;
	vec3 viewPos;
	vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 T = mat3(model) * aTangent;
	vec3 N = mat3(transpose(inverse(model))) * aNormal;
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N); //world -> tangent
	vs_out.vertexPos = TBN * mat3(model) * aPos;
	vs_out.lightPos = TBN * lightPos;
	vs_out.viewPos = TBN * viewPos;
	vs_out.texCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}