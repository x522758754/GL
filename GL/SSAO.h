#pragma once
#include "App.h"
#include <Utils.h>
#include <shader.h>
#include <model.h>
#include <random>


class SSAO : public App
{
	const unsigned int NR_LIGHTS = 32;
	const unsigned int N_SAMPLE_SIZE = 64;
	const unsigned int N_NOISE_SIZE = 16;

	Model m;
	Shader shaderGeometry, shaderCalc, shaderBlur, shaderLight;
	unsigned int fbo, rbo, fboSSAO, fboBlur;
	unsigned int tex_diffuse1, tex_specular1, texs[3], tex_Noise, tex_ssaoColorbuffer, tex_blurBuffer;
	std::vector<glm::vec3> lightPositions, lightColors;
	std::uniform_real_distribution<float> randomFloats;
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;
	//light info
	glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	void start()
	{
		//random normal-oriented hemisphere, generate sample kernel
		randomFloats = std::uniform_real_distribution<float>(0.0, 1.0);
		for (unsigned int i=0; i != N_SAMPLE_SIZE; ++i)
		{
			glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = (float)i / N_SAMPLE_SIZE;
			scale = lerp(0.1f, 1.0f, scale * scale);
			ssaoKernel.push_back(sample);
		}

		// random kernel rotations, generate noise texture
		for (unsigned int i=0; i < N_NOISE_SIZE; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			ssaoNoise.push_back(noise);
		}
		glGenTextures(1, &tex_Noise);
		glBindTexture(GL_TEXTURE_2D, tex_Noise);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
		shaderGeometry = Shader("shaders/ssaoGeometry.vs", "shaders/ssaoGeometry.fs");
		shaderCalc = Shader("shaders/hdr.vs", "shaders/ssaoCalc.fs");
		shaderBlur = Shader("shaders/hdr.vs", "shaders/ssaoBlur.fs");
		shaderLight = Shader("shaders/hdr.vs", "shaders/ssaoLight.fs");

		shaderCalc.use();
		shaderCalc.setInt("texture_gPositionDepth", 0);
		shaderCalc.setInt("texture_gNormal", 1);
		shaderCalc.setInt("texture_gAlbedoSpec", 2);
		shaderCalc.setInt("texture_Noise", 3);

		shaderBlur.use();
		shaderBlur.setInt("texture_ssaoInput", 0);

		shaderLight.use();
		shaderLight.setInt("texture_gPositionDepth", 0);
		shaderLight.setInt("texture_gNormal", 1);
		shaderLight.setInt("texture_gAlbedo", 2);
		shaderLight.setInt("texture_ssaoBlur", 3);

		//models
		m = Model("models/nanosuit/nanosuit.obj");

		//textures
		tex_diffuse1 = Utils::loadTexture("textures/wood.png", false);

		//1fbo geometry pass
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glGenRenderbuffers(1, &rbo);//render buffer object
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glGenTextures(3, texs);//texture object
		for (int i = 0; i != 3; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, texs[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texs[i], 0);
		}
		unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//2fbo calc 
		glGenFramebuffers(1, &fboSSAO);
		glBindFramebuffer(GL_FRAMEBUFFER, fboSSAO);
		glGenTextures(1, &tex_ssaoColorbuffer);
		glBindTexture(GL_TEXTURE_2D, tex_ssaoColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_ssaoColorbuffer, 0);
		//3fbo blur
		glGenFramebuffers(1, &fboBlur);
		glBindFramebuffer(GL_FRAMEBUFFER, fboBlur);
		glGenTextures(1, &tex_blurBuffer);
		glBindTexture(GL_TEXTURE_2D, tex_blurBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_blurBuffer, 0);

		//opengl status
		glEnable(GL_DEPTH_TEST);
	}

	void render(float time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//1.geometry pass: render scene's geometry/color data into gbuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderGeometry.use();
		glm::mat4 model, view, projection;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
		shaderGeometry.setMat4("model", model);
		shaderGeometry.setMat4("view", view);
		shaderGeometry.setMat4("projection", projection);
		m.Draw(shaderGeometry);

		//2.generate ssao texture
		glBindFramebuffer(GL_FRAMEBUFFER, fboSSAO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderCalc.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texs[2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, tex_Noise);
		for (int i = 0; i != N_SAMPLE_SIZE; ++i)
		{
			shaderCalc.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
		shaderCalc.setMat4("projection", projection);
		renderQuad();

		//3.blur ssao texture to remove noise
		glBindFramebuffer(GL_FRAMEBUFFER, fboBlur);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderBlur.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_ssaoColorbuffer);
		renderQuad();

		//4. lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderLight.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texs[2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, tex_blurBuffer);
		const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.09;
		const float quadratic = 0.032;
		shaderLight.setFloat("light.linear", linear);
		shaderLight.setFloat("light.quadratic", quadratic);
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
		shaderLight.setVec3("light.position", lightPosView);
		shaderLight.setVec3("light.color", lightColor);
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