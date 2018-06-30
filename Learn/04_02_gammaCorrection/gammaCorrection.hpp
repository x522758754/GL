#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//ע��
///ֱ�����ڣ����ǻ�һֱ�����������еĹ������������Կռ��н��еģ���ע��GammaΪ1����
///�����ջ���Ҫ�����е���ɫ������������ϣ������������õ�������ɫ�͹��ձ���������Ƕ��������ǲ���ȷ�ģ������ǵļ������Ϻ����ܹ���ȷ����ʾ��

//Gamma
///�������Ⱥ͸�֪���ȵ��������ڣ��������Ȼ��ڹ�����������֪���Ȼ����˵ĸо�

///2.2ͨ�����Ǵ������ʾ�豸�Ĵ��ƽ��gammaֵ������gamma2.2����ɫ�ռ����sRGB��ɫ�ռ䡣
///ÿ����������gamma���߶�������ͬ������gamma2.2�ڴ�����������ϱ��ֶ�����

///gammaУ������������ɫ�ռ�ת��Ϊ�����Կռ䣬���������һ������gammaУ���Ǽ�����Ҫ�ġ�

//gammaУ��1(����Ҳ��ɥʧһЩ����Ȩ)��ʹ��OpenGL�ڽ���sRGB֡���塣 �Լ���������ɫ���н���gammaУ����
///glEnable(GL_FRAMEBUFFER_SRGB);

//gammaУ��2(����ȫ�Ŀ���Ȩ):
///������ÿ�����������ɫ�����е����Ӧ��gammaУ���������ڷ��͵�֡����ǰ����ɫ�ͱ�У���ˡ�

//sRGB�ռ䶨���gamma�ӽ���2.2


class gammaCorrection : public app
{
	GLuint cubeVAO, planeVAO;
	Shader shader, lightShader;
	GLuint texture_diffuse1, texture_specular1, texture_diffuse1_gamma, texture_specular_gamma;

	int switchOn = 0;
	bool gamma = false;

	void start()
	{
		shader = Shader("_shaders/_0401_blinnPhong.vs", "_shaders/_0401_blinnPhong.fs");
		lightShader = Shader("_shaders/_011_light.vs", "_shaders/_011_light.fs");


		texture_diffuse1 = Utils::loadTexture("_texs/wood.png");
		texture_specular1 = Utils::loadTexture("_texs/wood.png");

		texture_diffuse1_gamma = Utils::loadTexture("_texs/wood.png", true);
		texture_specular_gamma = Utils::loadTexture("_texs/wood.png", true);

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos(1.2f, 0.0f, 2.0f);


		glm::mat4 model, view, projection;
		model = glm::mat4();
		view = cam->GetViewMatrix();
		projection = glm::perspective(cam->get_fovy(), 1.f * cfg.scrWidth / cfg.scrHeight, .1f, 100.f);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		shader.setVec3("lightPos", lightPos);
		shader.setVec3("lightColor", glm::vec3(0.5, 0.5, 0.1));
		shader.setVec3("eyePos", cam->get_position());
		shader.setFloat("shininess", 32);
		shader.setInt("switchOn", switchOn);

		shader.setInt("texture_diffuse1", 0);
		shader.setInt("texture_specular1", 1);
		shader.setBool("gamma", gamma);

		glActiveTexture(GL_TEXTURE0);
		if (gamma)
			glBindTexture(GL_TEXTURE_2D, texture_diffuse1_gamma);
		else
			glBindTexture(GL_TEXTURE_2D, texture_diffuse1);

		glActiveTexture(GL_TEXTURE1);
		if (gamma)
			glBindTexture(GL_TEXTURE_2D, texture_specular_gamma);
		else
			glBindTexture(GL_TEXTURE_2D, texture_specular1);

		drawPlane(&planeVAO);

		lightShader.use();

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));

		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		drawCube(&cubeVAO);
	}

	void shutdown()
	{

	}

	void onKey(int key, int action)
	{
		if (action)
		{
			switch (key)
			{
			case GLFW_KEY_SPACE:
				switchOn = switchOn > 0 ? 0 : 2;
				break;
			case  GLFW_KEY_G:
				gamma = !gamma;
				break;
			}
		}
	}
};