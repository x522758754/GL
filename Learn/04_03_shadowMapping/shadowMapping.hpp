#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//��Ӱӳ��
///�Թ��λ��Ϊ�ӽǽ�����Ⱦ�������ܿ����Ķ���������������������һ��������Ӱ֮��
//��Ȼ���
///������ӽ��£���Ӧ��һ��ƬԪ��һ��0��1֮������ֵ
//��Ӱ��ͼ/�����ͼ(depth map)
///�ӹ�Դ��͸���ӽ��£��������ֵ�Ľ���洢��������

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

		//��ֻ��Ҫ��Ⱦ�����������Ϣ����ɫ����û��
		///֡�����������ȫ��������ɫ�����
		///������Ҫ��ʾ����OpenGL���ǲ������κ���ɫ���ݽ�����Ⱦ
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


		// 1.������Ⱦ�����������ͼ
		
		///��Ҫ�ı��ӿڵĲ�������ӦdepthMap�ĳߴ�
		///����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
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

		// 2.��Ⱦ�����ͼ����Ļ
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