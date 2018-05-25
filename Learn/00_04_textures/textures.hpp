#include "app.h"
#include "shader.h"
#include "Utils.h"

class textures : public app
{
	//纹理环绕方式(TEXTURE WRAP);定义了一种当纹理顶点超出范围(0, 1)时指定OpenGL如何采样纹理的模式。
	///GL_REPEAT			对纹理的默认行为，重复纹理
	///GL_MIRRORED_REPEAT	每次重复都是镜像放置的
	///GL_CLAMP_TO_EDGE		纹理坐标被约束在0,1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘拉伸效果
	///GL_CLAMP_TO_BORDER	超出的坐标为用户指定的边缘颜色,调用 glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//纹理过滤(TEXTURE FILTER):定义了一种当有多种纹素选择时指定OpenGL如何采样纹理的模式。
	///注:纹理坐标不依赖于分辨率,纹理像素(Texture Pixel或 Texel）
	///GL_NEAREST			Nearest Neighbor Filtering；OpenGL会选择中心点最接近纹理坐标的那个像素；颗粒状图案
	///GL_LINEAR			(Bi)linear Filtering；基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大；更平滑图案

	//Mipmap
	///GL_NEAREST_MIPMAP_NEAREST	使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
	///GL_LINEAR_MIPMAP_NEAREST	使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
	///GL_NEAREST_MIPMAP_LINEAR	在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
	///GL_LINEAR_MIPMAP_LINEAR	在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
	
	//纹理坐标
	///(0,1)	(0.5,1)		(1,1）
	///
	///(0,0)				(1,0)
	unsigned int texture, texture1;
	unsigned int VAO;
	Shader shader;

	void start()
	{
		float vertices[] = {
			//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		unsigned int indices[] = { // 注意索引从0开始! 
			0, 1, 3, // 第一个三角形
			1, 2, 3  // 第二个三角形
		};

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		texture = Utils::loadTexture("_texs/container.jpg");
		texture1 = Utils::loadTexture("_texs/awesomeface.png");

		shader = Shader("_shaders/_004_textures.vs", "_shaders/_004_textures.fs");
		shader.use();
		shader.setInt("ourTexture", 0);
		shader.setInt("texture1", 1);

	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.2f, .2f, .2f, 1.f);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
};