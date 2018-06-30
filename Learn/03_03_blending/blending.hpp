#include "app.h"
#include "shader.h"
#include <model.h>
#include <vector>
#include <map>

//blending ʵ������͸���ȵ�һ�ּ���

//��Ⱦ͸����أ�
///����Ƭ�Σ�discard������ϣ�blending��

//���ڲ�����ȫ͸�������壬���ǿ���ѡ����͸����Ƭ�ζ����ǻ�����ǣ������ͽ������Щͷ�۵����⣨û��������⣩��

//ʹ�û�ϵ�ԭ�� ע����Ƶ�˳��
/// �Ȼ������в�͸�������塣
/// ������͸������������
/// ��˳���������͸�������塣

class blending : public app
{
	Model m;
	Shader shader, shader1;

	unsigned int transparentVAO, transparentVBO;

	GLuint texture;
	std::map<float, glm::vec3> sorted;

	std::vector<glm::vec3> windows
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	void start()
	{
		//m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_033_blending_sort.vs", "_shaders/_033_blending_sort.fs");

		shader.use();
		shader.setInt("texture_diffuse1", 0);

		texture = Utils::loadTexture("_texs/window.png");

		float transparentVertices[] = {
			// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
			1.0f,  0.5f,  0.0f,  1.0f,  0.0f
		};

		glGenVertexArrays(1, &transparentVAO);
		glGenBuffers(1, &transparentVBO);
		glBindVertexArray(transparentVAO);
		glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);


		for (unsigned int i = 0; i < windows.size(); i++)
		{
			float distance = glm::length(cam->get_position() - windows[i]);
			sorted[distance] = windows[i];
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glm::mat4 view;
		// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glm::mat4 model;
		shader.setMat4("model", model);
		glBindVertexArray(transparentVAO);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::mat4();
			model = glm::translate(model, it->second);
			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}


};