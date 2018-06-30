#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//注：
///直到现在，我们还一直假设我们所有的工作都是在线性空间中进行的（译注：Gamma为1），
///但最终还是要把所有的颜色输出到监视器上，所以我们配置的所有颜色和光照变量从物理角度来看都是不正确的，在我们的监视器上很少能够正确地显示。

//Gamma
///物理亮度和感知亮度的区别在于，物理亮度基于光子数量，感知亮度基于人的感觉

///2.2通常是是大多数显示设备的大概平均gamma值。基于gamma2.2的颜色空间叫做sRGB颜色空间。
///每个监视器的gamma曲线都有所不同，但是gamma2.2在大多数监视器上表现都不错。

///gamma校正将把线性颜色空间转变为非线性空间，所以在最后一步进行gamma校正是极其重要的。

//gamma校正1(我们也会丧失一些控制权)：使用OpenGL内建的sRGB帧缓冲。 自己在像素着色器中进行gamma校正。
///glEnable(GL_FRAMEBUFFER_SRGB);

//gamma校正2(有完全的控制权):
///我们在每个相关像素着色器运行的最后应用gamma校正，所以在发送到帧缓冲前，颜色就被校正了。

//sRGB空间定义的gamma接近于2.2


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