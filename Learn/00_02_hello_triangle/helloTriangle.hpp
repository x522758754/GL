#include "app.h"
#include "shader.h"
#include  <direct.h>  
//1.vertex shader
//2.primitive assembly
//3.geometry shader
//4.rasterization stage
// normalized device coordinates,NDC,顶点坐标在vertex shader处理之后，它们就应该是ndc了,坐标xyz 在(-1.0,1.0)
//通过glViewport函数进行Viewport Transform, ndc => screen-space coordinates

class helloTriangle : public app
{
	///顶点数组对象，类似VBO，OpenGL的核心模式要求我们使用VAO,
	///存储缓冲区和顶点属性状态。
	///链接顶点属性
	unsigned int vertexArrayObject;

	Shader shader;

	void start()
	{
		// 		char   buffer[MAX_PATH];
		// 		_getcwd(buffer, MAX_PATH);
		// 		printf("The   current   directory   is:   %s ", buffer);

		float vertices[9] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};

		float vertices1[] = {
			0.5f, 0.5f, 0.0f,   // 右上角
			0.5f, -0.5f, 0.0f,  // 右下角
			-0.5f, -0.5f, 0.0f, // 左下角
			-0.5f, 0.5f, 0.0f   // 左上角
		};

		//1
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		//2
		///顶点缓冲对象,管理GPU内存(显存)中存储的顶点数据,使用缓冲对象的好处可以一次性发送一批数据到GPU
		///从CPU把数据从内存发送到显存中,之后vertex shader几乎能立即访问顶点
		unsigned int vertexBufferObject;
		glGenBuffers(1, &vertexBufferObject); //分配1个缓冲对象
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		///GL_STATIC_DRAW ：数据不会或几乎不会改变。
		///GL_DYNAMIC_DRAW：数据会被改变很多。
		///GL_STREAM_DRAW ：数据每次绘制时都会改变。
		///三角形的位置数据不会改变，每次渲染调用时都保持原样，所以它的使用类型最好是GL_STATIC_DRAW。如果，
		///比如说一个缓冲中的数据将频繁被改变，那么使用的类型就是GL_DYNAMIC_DRAW或GL_STREAM_DRAW，这样就能确保显卡把数据放在能够高速写入的内存部分。
		//在GPU中进行内存分配，存储数据
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

		//3
		unsigned int indices[] = { // 注意索引从0开始! 
			0, 1, 3, // 第一个三角形
			1, 2, 3  // 第二个三角形
		};
		///一个存储索引供索引化绘制使用的缓冲对象
		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//4
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shader = Shader("_shaders/_002_helloTriangle.vs", "_shaders/_002_helloTriangle.fs");
		shader.use();
	}


	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.2f, .2f, .2f, 1.f);

 		glBindVertexArray(vertexArrayObject);
// 		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
};