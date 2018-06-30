#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>

//因为每一根草仅仅是由几个三角形构成，渲染几乎是瞬间完成的，但上千个渲染函数调用却会极大地影响性能。例：
///glBindVertexArray(VAO);
///glDrawArrays(GL_TRIANGLES, 0, amount_of_vertices);
///绘制调用过多，达到性能瓶颈
//与绘制顶点本身相比，使用glDrawArrays或glDrawElements函数告诉GPU去绘制顶点数据会消耗会更多的性能，
//因为OpenGL在绘制顶点数据之前需要做很多准备工作(比如告诉GPU该从哪个缓冲读取数据，从哪寻找顶点属性，而且这些都是在相对缓慢的CPU到GPU总线(CPU to GPU Bus)上进行的）。
//所以，即便渲染顶点非常快，命令GPU去渲染却未必。

//实例化:我们能够将数据一次性发送给GPU，然后使用一个绘制函数让OpenGL利用这些数据绘制多个物体，就会更方便了
///让我们使用一个渲染调用来绘制多个物体，来节省每次绘制物体时CPU -> GPU的通信，它只需要一次即可。

//gl_InstanceID
///在使用实例化渲染调用时，gl_InstanceID会从0开始，在每个实例被渲染时递增1。

class instancing : public app
{
	GLuint VAO, VBO;
	Shader shader;

	glm::vec2 translations[100];


	void start()
	{
		float quadVertices[] = {
			// 位置          // 颜色
			-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
			0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
			-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

			-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
			0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
			0.05f,  0.05f,  0.0f, 1.0f, 1.0f
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		shader = Shader("_shaders/_0310_instancing.vs", "_shaders/_0310_instancing.fs");

		int index = 0;
		float offset = 0.1f;
		for (int y = -10; y < 10; y += 2)
		{
			for (int x = -10; x < 10; x += 2)
			{
				glm::vec2 translation;
				translation.x = (float)x / 10.0f + offset;
				translation.y = (float)y / 10.0f + offset;
				translations[index++] = translation;
			}
		}
	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		for (unsigned int i = 0; i < 100; i++)
		{
			std::stringstream ss;
			std::string index;
			ss << i;
			index = ss.str();
			shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
			//但是如果我们要渲染远超过100个实例的时候（这其实非常普遍），我们最终会超过最大能够发送至着色器的uniform数据大小上限。
			//它的一个代替方案是实例化数组(Instanced Array)，它被定义为一个顶点属性（能够让我们储存更多的数据），仅在顶点着色器渲染一个新的实例时才会更新。
		}
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

	}

	void shutdown()
	{

	}
};