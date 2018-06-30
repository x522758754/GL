#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"

//Blinn-Phong
///Phong和Blinn-Phong着色之间另一个细微差别是，半程向量和表面法线之间的角度经常会比视线和反射向量之间的夹角更小。
///结果就是，为了获得和Phong着色相似的效果，必须把发光值参数设置的大一点。通常的经验是将其设置为Phong着色的发光值参数的2至4倍。

class advancedLighting : public app
{
	GLuint cubeVAO, planeVAO;
	Shader shader, lightShader;
	GLuint texture_diffuse1, texture_specular1;

	int switchOn = 0;

	void start()
	{
		shader = Shader("_shaders/_0401_blinnPhong.vs", "_shaders/_0401_blinnPhong.fs");
		lightShader = Shader("_shaders/_011_light.vs", "_shaders/_011_light.fs");


		texture_diffuse1 = Utils::loadTexture("_texs/wood.png");
		texture_specular1 = Utils::loadTexture("_texs/wood.png");

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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse1);

		glActiveTexture(GL_TEXTURE1);
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
			}
		}
	}
};