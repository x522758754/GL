#pragma once
#include "App.h"
#include <Utils.h>
#include <shader.h>
#include <model.h>

class GrassWave :public App
{
	Shader shaderGrass;

	unsigned int texMap, alphaMap;
	
	Model m;

	unsigned int quadVAO = 0, quadVBO;
	void renderQuad()
	{
		if (0 == quadVAO)
		{
			//position
			glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
			glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
			glm::vec3 pos3(1.0f, -1.0f, 0.0f);
			glm::vec3 pos4(1.0f, 1.0f, 0.0f);
			// normal vector
			glm::vec3 nm(0.0f, 0.0f, 1.0f);
			// texture coordinates
			glm::vec2 uv1(0.0f, 1.0f);
			glm::vec2 uv2(0.0f, 0.0f);
			glm::vec2 uv3(1.0f, 0.0f);
			glm::vec2 uv4(1.0f, 1.0f);
			//colors
			glm::vec3 color1(1.0f, 1.0f, 1.0f);
			glm::vec3 color2(0.0f, 0.0f, 0.0f);
			glm::vec3 color3(0.0f, 0.0f, 0.0f);
			glm::vec3 color4(1.0f, 1.0f, 1.0f);

			glGenVertexArrays(1, &quadVAO);
			glBindVertexArray(quadVAO);
			glGenBuffers(1, &quadVBO);
			glGenVertexArrays(1, &quadVAO);
			glBindVertexArray(quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

			float quadVertices[] = {
				// positions            // normal         // texcoords  // color
				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, color1.r, color1.g, color1.b,
				pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, color2.r, color2.g, color2.b,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, color3.r, color3.g, color3.b,

				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, color1.r, color1.g, color1.b,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, color3.r, color3.g, color3.b,
				pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, color4.r, color4.g, color4.b,
			};

			glGenVertexArrays(1, &quadVAO);
			glBindVertexArray(quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void start()
	{
		//shader
		shaderGrass = Shader("shaders/grassWave.vs", "shaders/grassWave.fs");
		shaderGrass.use();
		shaderGrass.setInt("texture_diffuse1", 0);

		//models
		//m = Model("models/Grass/grass_02.3DS");

		//opengl status
		glEnable(GL_DEPTH_TEST);
	}

	void render(float time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1, 0.3, 0.1, 1);

		glm::mat4 model, view, projection;
		//model = glm::scale(model, glm::vec3(0.002f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);

		shaderGrass.use();
		shaderGrass.setMat4("model", model);
		shaderGrass.setMat4("view", view);
		shaderGrass.setMat4("projection", projection);

		shaderGrass.setFloat("time", time);
		shaderGrass.setVec3("dstPos", glm::vec3(1.0));

		//m.Draw(shaderGrass);
		renderQuad();
	}
};