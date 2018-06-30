#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//阴影映射
///以光的位置为视角进行渲染，我们能看到的东西将被点亮，看不见的一定是在阴影之中
//深度缓冲
///在相机视角下，对应于一个片元的一个0到1之间的深度值
//阴影贴图/深度贴图(depth map)
///从光源的透视视角下，并把深度值的结果存储到纹理中

#define depthMapWidth 1024
#define depthMapHeight 1024

class shadowMapping : public app
{
	GLuint cubeVAO, planeVAO, quadVAO;
	Shader shader, shadowShader;
	GLuint depthMap, texture_diffuse1, texture_specular1;
	GLuint depthMapFBO;

	void start()
	{
		shader = Shader("_shaders/_0403_shadowMapping.vs", "_shaders/_0403_shadowMapping.fs");
		shadowShader = Shader("_shaders/_0403_screenDpeth.vs", "_shaders/_0403_screenDpeth.fs");

		texture_diffuse1 = Utils::loadTexture("_texs/wood.png");
		texture_specular1 = Utils::loadTexture("_texs/wood.png");

		depthMapFBO = genfboDepth(&depthMap, depthMapWidth, depthMapHeight);

		//当只需要渲染场景的深度信息，颜色缓冲没用
		///帧缓冲对象不是完全不包含颜色缓冲的
		///我们需要显示告诉OpenGL我们不进行任何颜色数据进行渲染
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glm::vec3 lightPos(12.0f, 13.0f, 12.0f);

		glm::vec3 cubePositions[] =
		{
			glm::vec3(0.0f, 3.5f, 0.0),
			glm::vec3(-1.0f, 1.0f, -1.0),
			glm::vec3(1.0f, 0.0f, -2.0),
		};

		glm::mat4 model, view, projection;
		model = glm::mat4();
		view = glm::lookAt(lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.0f, 0.f));
		projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, -1.f, 100.f);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// 1.首先渲染场景的深度贴图
		
		///需要改变视口的参数以适应depthMap的尺寸
		///如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
		glViewport(0, 0, depthMapWidth, depthMapHeight); 

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		drawPlane(&planeVAO);

		for (int i=0; i != cubePositions->length(); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			shader.setMat4("model", model);
			drawCube(&cubeVAO);
		}

		// 2.渲染深度贴图到屏幕
		shadowShader.use();
		shadowShader.setInt("screenTexture", 0);

		glViewport(0, 0, cfg.scrWidth, cfg.scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		drawQuad(&quadVAO);
	}

	void shutdown()
	{

	}

};