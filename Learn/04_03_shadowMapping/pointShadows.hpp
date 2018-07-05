#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>


//�����Ӱ(point shadows)
///�������ǵĽ������ڸ��ַ������ɶ�̬��Ӱ������������������ڵ��Դ���������з����ϵ���Ӱ��

//���������������ͼ
///Ϊ����һ������Χ�����ֵ����������ͼ�����Ǳ�����Ⱦ����6�Σ�ÿ��һ���档��Ȼ��Ⱦ����6����Ҫ6����ͬ����ͼ����ÿ�ΰ�һ����ͬ����������ͼ�渽�ӵ�֡��������ϡ�
///���ܺķ�������Ϊһ�������ͼ����Ҫ���кܶ���Ⱦ���á�����̳������ǽ�ת��ʹ�������һ��С������������£�������ɫ����������ʹ��һ����Ⱦ���������������������ͼ��

#define depthMapWidth 1024
#define depthMapHeight 1024

class pointShadows : public app
{
	GLuint cubeVAO, planeVAO, quadVAO;
	Shader shader, shadowShader;
	GLuint depthMap, texture_diffuse1, texture_specular1, depthCubemap;
	GLuint depthMapFBO;
	glm::vec3 lightPos;

	bool shadows = true;


	void start()
	{
		lightPos = glm::vec3(0.f); //���renderscene()

		cam->set_position(lightPos);

		shader = Shader("_shaders/_0403_pointShadows_depth.vs", "_shaders/_0403_pointShadows_depth.fs", "_shaders/_0403_pointShadows_depth.gs");
		shadowShader = Shader("_shaders/_0403_pointShadows_blinnPhong.vs", "_shaders/_0403_pointShadows_blinnPhong.fs");

		texture_diffuse1 = Utils::loadTexture("_texs/wood.png");
		texture_specular1 = Utils::loadTexture("_texs/wood.png");

		depthMapFBO = genfboDepthCube(&depthCubemap, depthMapWidth, depthMapHeight);

		//������һ�������������ͼʱ����ֻ�������ֵ��
		//�������Ǳ�����ʽ����OpenGL���֡������󲻻���Ⱦ��һ����ɫ�����
		///֡�����������ȫ��������ɫ�����
		///������Ҫ��ʾ����OpenGL���ǲ������κ���ɫ���ݽ�����Ⱦ
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		//lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;

		float near_plane = 1.0f;
		float far_plane = 100.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)depthMapWidth / (float)depthMapHeight, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1.������Ⱦ�����������ͼ

		///��Ҫ�ı��ӿڵĲ�������ӦdepthMap�ĳߴ�
		///����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
		glViewport(0, 0, depthMapWidth, depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4();
		shader.use();
		shader.setMat4("model", model);
		shader.setFloat("far_plane", far_plane);
		shader.setVec3("lightPos", lightPos);
		for (unsigned int i = 0; i < 6; ++i)
			shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

		renderScene(shader);

		// 2.��Ⱦ�����ͼ����Ļ
		glViewport(0, 0, cfg.scrWidth, cfg.scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(cam->get_fovy()), (float)cfg.scrWidth / (float)cfg.scrHeight, 0.1f, 100.0f);
		glm::mat4 view = cam->GetViewMatrix();

		shadowShader.use();
		shadowShader.setInt("depthMap", 0);
		shadowShader.setInt("texture_diffuse1", 1);
		shadowShader.setInt("texture_specular1", 2);
		shadowShader.setVec3("lightPos", lightPos);
		shadowShader.setVec3("viewPos", cam->get_position());
		shadowShader.setFloat("far_plane", far_plane);
		shadowShader.setBool("shadows", shadows);
		shadowShader.setMat4("projection", projection);
		shadowShader.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture_specular1);

		renderScene(shadowShader);
	}


	void renderScene(const Shader &shader)
	{
		// room cube
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(25.0f));
		shader.setMat4("model", model);
		glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
		shader.setBool("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
		drawCube(&cubeVAO);
		shader.setBool("reverse_normals", 0); // and of course disable it
		//glEnable(GL_CULL_FACE);
		// cubes
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.75f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.75f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
	}

	void shutdown()
	{

	}

	void onKey(int key, int action)
	{
		app::onKey(key, action);
		if (action)
		{
			switch (key)
			{
			case GLFW_KEY_SPACE:
				shadows = !shadows;
				break;
			}
		}
	}

};