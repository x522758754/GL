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
	//T B N 用模型空间向量表示,做如下处理将它们转换到世界空间下
	vec3 T = normalize(mat3(model) * aTangent);
	vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);
	vec3 B = cross(N, T);
	//T B N经过空间变换，此时已从世界坐标下的向量
	//由此时的TBN构成矩阵 可以将切线空间的点 =》 世界空间
	mat3 TBN = mat3(T, B, N); //world -> tangent
	// Vworld = TBN * Vtangent
	// TBN-1 Vworld = Vtangent
	//所以求出TBN的逆矩阵则能将world =》 tangent
	//由于TBN是正交矩阵，TBN的逆等于其转置矩阵
	TBN = transpose(TBN);
	vs_out.vertexPos = TBN * mat3(model) * aPos;
	vs_out.lightPos = TBN * lightPos;
	vs_out.viewPos = TBN * viewPos;
	vs_out.texCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}