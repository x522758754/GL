#include "app.h"
#include "shader.h"
#include  <direct.h>  
//1.vertex shader
//2.primitive assembly
//3.geometry shader
//4.rasterization stage
// normalized device coordinates,NDC,顶点坐标在vertex shader处理之后，它们就应该是ndc了,坐标xyz 在(-1.0,1.0)
//通过glViewport函数进行Viewport Transform, ndc => screen-space coordinates

/// 顶点数组对象：Vertex Array Object，
/// 顶点缓冲对象：Vertex Buffer Object
/// 索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO

///理解：VAO 指定当前绘制的哪个模型；VAO 可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
///		 对于不同的模型（不同的顶点属性）或者不同的属性配置(不一样的VBO)，只需要绑定不同的VAO。
///		 VBO依附于VAO，管理、配置这个模型的顶点数据； 在GPU上创建内存存储顶点数据，使用缓冲对象的好处：是我们可以一次性的发送一大批数据到显卡上，而不是每个顶点发送一次。
///																										从CPU把数据发送到显卡相对较慢，所以只要可能我们都要尝试尽量一次性发送尽可能多的数据。
///																										当数据发送至显卡的内存中后，顶点着色器几乎能立即访问顶点，这是个非常快的过程。

///glDrawArrays：绘制函数，它使用当前激活的着色器，之前的定义顶点属性配置和VBO的顶点数据（通过VAO间绑定）来绘制图元。

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