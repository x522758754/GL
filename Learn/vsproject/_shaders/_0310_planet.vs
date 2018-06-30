#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
//当我们顶点属性的类型大于vec4时，就要多进行一步处理了。
//顶点属性最大允许的数据大小等于一个vec4。
//因为一个mat4本质上是4个vec4，我们需要为这个矩阵预留4个顶点属性。
//因为我们将它的位置值设置为3，矩阵每一列的顶点属性位置值就是3、4、5和6。
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0); 
	TexCoords = aTexCoords;
}