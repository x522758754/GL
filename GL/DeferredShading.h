#pragma once
#include "App.h"
#include <Utils.h>
#include <shader.h>
#include <model.h>

class DeferredShading: public App
{
	Model m;
	Shader shader, shaderGeometry, shaderLight;
	unsigned int fbo, rbo;
	unsigned int tex_diffuse1, tex_specular1, texs[3];
	std::vector<glm::vec3> lightPositions, lightColors;
	const unsigned int NR_LIGHTS = 32;

	void start()
	{
		//lightColors
		srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}

		//shaders
		shaderGeometry = Shader("shaders/multipleLightBlinn.vs", "shaders/deferredGeometry.fs");
		shaderLight = Shader("shaders/hdr.vs", "shaders/deferredLight.fs");

		shaderLight.use();
		shaderLight.setInt("texture_gPosition", 0);
		shaderLight.setInt("texture_gNormal", 1);
		shaderLight.setInt("texture_gAlbedoSpec", 2);

		//models
		m = Model("models/nanosuit/nanosuit.obj");

		//textures
		tex_diffuse1 = Utils::loadTexture("textures/wood.png", false);

		//frame buffer object
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//render buffer object
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		//texture object
		glGenTextures(3, texs);
		for (int i = 0; i != 3; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, texs[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texs[i], 0);
		}
		unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//opengl status
		glEnable(GL_DEPTH_TEST);
	}

	void render(float time)
	{
 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderGeometry.use();
		glm::mat4 model, view, projection;
		model = glm::scale(model, glm::vec3(0.2));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
		shaderGeometry.setMat4("model", model);
		shaderGeometry.setMat4("view", view);
		shaderGeometry.setMat4("projection", projection);
		m.Draw(shaderGeometry);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderLight.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texs[2]);
		for (int i=0; i != NR_LIGHTS; ++i)
		{
			shaderLight.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
			shaderLight.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
		}
		shaderLight.setVec3("viewPos", camera.Position);
		renderQuad();
	}

	// -----------------------------------------
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

};