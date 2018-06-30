#include "app.h"
#include "shader.h"
#include "Utils.h"

//因为这些形状是在GPU的超快硬件中动态生成的，这会比在顶点缓冲中手动定义图形要高效很多。
//因此，几何缓冲对简单而且经常重复的形状来说是一个很好的优化工具，比如体素(Voxel)世界中的方块和室外草地的每一根草。

class geometryShader : public app
{
	Shader shader;
	GLuint VAO, VBO;

	void start()
	{
		float points[] = {
			-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
			0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
			-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		shader = Shader("_shaders/_039_geometry.vs", "_shaders/_039_geometry.fs", "_shaders/_039_geometry.gs");
	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 4);
	}

	void shutdown()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}


};