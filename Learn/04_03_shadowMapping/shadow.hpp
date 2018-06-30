#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//阴影失真成因
///因为阴影贴图受限于解析度，在距离光源比较远的情况下，多个片元可能从深度贴图同一个值中采样

//阴影偏移
///们简单的对表面的深度（或深度贴图）应用一个偏移量，这样片元就不会被错误地认为在表面之下了

//正交 vs 投影
///在渲染深度贴图的时候，正交(Orthographic)和投影(Projection)矩阵之间有所不同。
///正交投影矩阵并不会将场景用透视图进行变形，所有视线 / 光线都是平行的，这使它对于定向光来说是个很好的投影矩阵。
///然而透视投影矩阵，会将所有顶点根据透视关系进行变形，结果因此而不同。下图展示了两种投影方式所产生的不同阴影区域：

//定向阴影映射，深度（阴影）贴图生成自定向光的视角

#define depthMapWidth 1024
#define depthMapHeight 1024

class shadow : public app
{
	GLuint cubeVAO, planeVAO, quadVAO;
	Shader shader, shadowShader;
	GLuint depthMap, texture_diffuse1, texture_specular1;
	GLuint depthMapFBO;

	void start()
	{
		cubeVAO = 0, planeVAO = 0, quadVAO = 0;

		cam->set_position(glm::vec3(2.f, 3.f, 2.f));
		
		shader = Shader("_shaders/_0403_shadowMapping.vs", "_shaders/_0403_shadowMapping.fs");
		shadowShader = Shader("_shaders/_0403_shadow.vs", "_shaders/_0403_shadow.fs");

		texture_diffuse1 = Utils::loadTexture("_texs/wood.png");
		texture_specular1 = Utils::loadTexture("_texs/container2_specular.png");

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
		glm::vec3 lightPos(2.0f * sin(time), 3.0f, 2.0f);

		glm::vec3 cubePositions[] =
		{
			glm::vec3(0.0f, 3.5f, 0.0),
			glm::vec3(-1.0f, 1.0f, -1.0),
			glm::vec3(1.0f, -.5f, -2.0),
		};

		glm::mat4 model, view, projection, lightView, lightProjection, lightSpaceMatrix;
		model = glm::mat4();
		view = cam->GetViewMatrix();
		projection = glm::perspective(cam->get_fovy(), 1.f * cfg.scrWidth / cfg.scrHeight, 1.f, 100.f);

		lightView = glm::lookAt(lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.0f, 0.f));
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, -1.f, 100.f);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// 1.首先渲染场景的深度贴图

		///需要改变视口的参数以适应depthMap的尺寸
		///如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
		glViewport(0, 0, depthMapWidth, depthMapHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT); //剔除正面，使用背面深度，防止深度检索造成阴影失真
		shader.use();
		shader.setMat4("projection", lightProjection);
		shader.setMat4("view", lightView);
		shader.setMat4("model", model);

		drawPlane(&planeVAO);

		for (int i = 0; i != cubePositions->length(); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			shader.setMat4("model", model);
			drawCube(&cubeVAO);
		}

		glCullFace(GL_BACK); //提出背面，默认

		// 2.渲染深度贴图到屏幕
		glViewport(0, 0, cfg.scrWidth, cfg.scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadowShader.use();
		shadowShader.setInt("depthMap", 0);
		shadowShader.setInt("texture_diffuse1", 1);
		shadowShader.setInt("texture_specular1", 2);
		shadowShader.setVec3("lightPos", lightPos);
		shadowShader.setVec3("viewPos", cam->get_position());
		shadowShader.setMat4("projection", projection);
		shadowShader.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture_specular1);

		model = glm::mat4();
		shadowShader.setMat4("model", model);
		shadowShader.setMat4("lightSpaceMatrix", lightProjection * lightView * model);

		drawPlane(&planeVAO);

		for (int i = 0; i != cubePositions->length(); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			shadowShader.setMat4("model", model);
			shadowShader.setMat4("lightSpaceMatrix", lightProjection * lightView * model);
			drawCube(&cubeVAO);
		}
	}

	void shutdown()
	{

	}

};