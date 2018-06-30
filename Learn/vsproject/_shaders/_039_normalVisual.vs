#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aNormal;
layout(location = 2)in vec2 aTexCoords;

out VS_OUT{
	vec2 texCoords;
	vec3 normal; //几何着色器接受的位置向量是剪裁空间坐标
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vs_out.texCoords = aTexCoords;
	vec3 dirNormal = mat3(transpose(inverse( view * model))) * aNormal;
	vs_out.normal = normalize(vec3(projection * vec4(dirNormal, 0.0)));
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}