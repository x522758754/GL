//天空盒
#version 330 core
layout(location=0)in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;
out vec4 clip;

void main()
{
	localPos = aPos;
	mat4 rotView = mat4(mat3(view)); //将视矩阵中的平移变换移除，remove translation from the view matrix;
	vec4 clipPos = projection * rotView * vec4(localPos, 1.0);
	clip = clipPos;
	//我们需要欺骗深度缓冲，让它认为天空盒有着最大的深度值1.0，只要它前面有一个物体，深度测试就会失败。
	//透视除法是在顶点着色器运行之后执行的，将gl_Position的xyz坐标除以w分量。我们又从深度测试小节中知道，相除结果的z分量等于顶点的深度值。
	//改变深度函数为glDepthFunc(GL_LEQUAL)， 默认的深度缓冲填写是GL_LESS才会写入
	gl_Position = vec4(clipPos.x, clipPos.y, clipPos.w, clipPos.w); //xyww ensures the depth value of the rendered cube fragments always end up at 1.0,the maximum depth value
	//gl_Position = projection * view * vec4(aPos, 1.0);
}