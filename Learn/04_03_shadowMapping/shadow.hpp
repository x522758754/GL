#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//��Ӱʧ�����
///��Ϊ��Ӱ��ͼ�����ڽ����ȣ��ھ����Դ�Ƚ�Զ������£����ƬԪ���ܴ������ͼͬһ��ֵ�в���

//��Ӱƫ��
///�Ǽ򵥵ĶԱ������ȣ��������ͼ��Ӧ��һ��ƫ����������ƬԪ�Ͳ��ᱻ�������Ϊ�ڱ���֮����

//���� vs ͶӰ
///����Ⱦ�����ͼ��ʱ������(Orthographic)��ͶӰ(Projection)����֮��������ͬ��
///����ͶӰ���󲢲��Ὣ������͸��ͼ���б��Σ��������� / ���߶���ƽ�еģ���ʹ�����ڶ������˵�Ǹ��ܺõ�ͶӰ����
///Ȼ��͸��ͶӰ���󣬻Ὣ���ж������͸�ӹ�ϵ���б��Σ������˶���ͬ����ͼչʾ������ͶӰ��ʽ�������Ĳ�ͬ��Ӱ����

//������Ӱӳ�䣬��ȣ���Ӱ����ͼ�����Զ������ӽ�

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


		// 1.������Ⱦ�����������ͼ

		///��Ҫ�ı��ӿڵĲ�������ӦdepthMap�ĳߴ�
		///����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
		glViewport(0, 0, depthMapWidth, depthMapHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT); //�޳����棬ʹ�ñ�����ȣ���ֹ��ȼ��������Ӱʧ��
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

		glCullFace(GL_BACK); //������棬Ĭ��

		// 2.��Ⱦ�����ͼ����Ļ
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