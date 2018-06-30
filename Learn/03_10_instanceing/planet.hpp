#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>

#define planetCount  5000

class planet : public app
{
	Shader shader;
	Model m;
	glm::mat4 *modelMatrices;

	void start()
	{
		cam->set_position(glm::vec3(0, 0, -200));
		shader = Shader("_shaders/_0310_planet.vs", "_shaders/_0310_planet.fs");
		m = Model("_models/planet/planet.obj");

		modelMatrices = new glm::mat4[planetCount];
		srand(glfwGetTime()); // initialize random seed	
		float radius = 150.0;
		float offset = 25.0f;
		for (unsigned int i = 0; i < planetCount; i++)
		{
			glm::mat4 model;
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)planetCount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
		}

		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, planetCount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		for (int i = 0; i != m.meshs.size(); ++i)
		{
			Mesh mesh = m.meshs[i];
			glBindVertexArray(mesh.VAO);

			//注： sizeof(glm::mat4) == 4 * sizeof(glm::vec4)， 每次偏移一个vec4
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(0 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);
		}

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 projection, view;
		projection = glm::perspective(cam->get_fovy(), 1.f * cfg.scrWidth / cfg.scrHeight, .1f, 10000.f);
		view = cam->GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		for (unsigned int i = 0; i != m.meshs.size(); ++i)
		{
			auto mesh = m.meshs[i];
			glBindVertexArray(mesh.VAO);
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, planetCount);
		}
	}

	void shutdown()
	{

	}
};