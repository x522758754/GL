#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "app.h"
#include "shader.h"
#include "Utils.h"

//light caster(Ͷ����):����Ͷ�䵽����Ĺ�Դ
///Directional Light


class multipleLights : public app
{
	unsigned int texture, texture1;
	unsigned int VAO;
	Shader shader, lightShader;

	void start()
	{
		cam->set_position(glm::vec3(0.0f, 0.0f, 3.0f));
		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		shader = Shader("_shaders/_016_multipleLights.vs", "_shaders/_016_multipleLights.fs");
		lightShader = Shader("_shaders/_011_light.vs", "_shaders/_011_light.fs");

		texture = Utils::loadTexture("_texs/container2.png");
		texture1 = Utils::loadTexture("_texs/container2_specular.png");

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(.2f, .2f, .2f, 1.f);

		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.2f, 0.0f, 2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
		};

		shader.use();
		shader.setVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));
		shader.setVec3("viewPos", cam->get_position());

		shader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		//shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		shader.setInt("material.diffuse", 0);
		//shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setInt("material.specular", 1);
		shader.setFloat("material.shininess", 32.0f);

		//direction
		shader.setVec3("dirLight.direction", glm::vec3(-1.2f, 0.0f, -2.0f));
		shader.setVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // �����յ�����һЩ�Դ��䳡��
		shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		//point 0
		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setVec3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // �����յ�����һЩ�Դ��䳡��
		shader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[0].constant", 1.f); //point
		shader.setFloat("pointLights[0].linear", 0.09f); //point
		shader.setFloat("pointLights[0].quadratic", 0.032f); //point
		//point 1
		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setVec3("pointLights[1].diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // �����յ�����һЩ�Դ��䳡��
		shader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[1].constant", 1.f); //point
		shader.setFloat("pointLights[1].linear", 0.09f); //point
		shader.setFloat("pointLights[1].quadratic", 0.032f); //point

		//spot
		shader.setVec3("spotLight.position", cam->get_position());
		shader.setVec3("spotLight.direction", cam->get_front());
		shader.setFloat("spotLight.cutOff", cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCutOff", cos(glm::radians(30.5f)));
		shader.setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setVec3("spotLight.diffuse", glm::vec3(0.88f, 0.8f, 0.f)); // �����յ�����һЩ�Դ��䳡��
		shader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 0.0f));

		glm::mat4 model;
		//model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		//shader.setMat4("model", model);

		glm::mat4 view;
		// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glm::mat4 mIT;
		mIT = glm::transpose(glm::inverse(model));
		shader.setMat4("mIT", mIT);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		for (int i = 0; i < pointLightPositions->length(); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
};