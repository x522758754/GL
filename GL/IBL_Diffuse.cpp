#include "IBL_Diffuse.h"



void IBL_Diffuse::start()
{
	//shader
	shaderPBR = Shader("shaders/pbrModel.vs", "shaders/pbrModel_IBL_Diffuse.fs");
	shaderMap = Shader("shaders/equirectangular2CubeMap.vs", "shaders/equirectangular2CubeMap.fs");
	shaderSkybox = Shader("shaders/skybox.vs", "shaders/skybox.fs");
	shaderIrradiance = Shader("shaders/cubemapConvolute.vs", "shaders/cubemapConvolute.fs");

	//light
	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 10.0f));
	lightPositions.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
	lightColors.push_back(glm::vec3(150.0f, 150.0f, 150.0f));
	lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
	lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
	lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

	//texture 
	albedoMap = Utils::loadTexture("textures/RustedIron/rustediron2_basecolor.png");
	normalMap = Utils::loadTexture("textures/RustedIron/rustediron2_normal.png");
	metallicMap = Utils::loadTexture("textures/RustedIron/rustediron2_metallic.psd");
	roughnessMap = Utils::loadTexture("textures/RustedIron/roughness.png");
	aoMap = Utils::loadTexture("textures/RustedIron/ao.png");

	equirectangularMap = Utils::loadHDRImgae("textures/PaperMill_Ruins_E/PaperMill_E_3k.hdr");

	shaderPBR.use();
	shaderPBR.setInt("albedoMap", 0);
	shaderPBR.setInt("normalMap", 1);
	shaderPBR.setInt("metallicMap", 2);
	shaderPBR.setInt("roughnessMap", 3);
	shaderPBR.setInt("aoMap", 4);
	shaderPBR.setInt("irradianceMap", 5);

	shaderMap.use();
	shaderMap.setInt("equirectangularMap", 0);

	shaderSkybox.use();
	shaderSkybox.setInt("envCubemap", 0);

	shaderIrradiance.use();
	shaderIrradiance.setInt("envCubemap", 0);

	//fbo
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envCubemapSize, envCubemapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (int i = 0; i != 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, envCubemapSize, envCubemapSize, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	//convert hdr equirectangular environment to cubemap equivalent
	glViewport(0, 0, envCubemapSize, envCubemapSize); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	shaderMap.use();
	shaderMap.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangularMap);

	for (int i = 0; i != 6; ++i)
	{
		//暂时注释,看看效果
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		shaderMap.setMat4("view", captureViews[i]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Utils::renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (int i = 0; i != 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapSize, irradianceMapSize, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceMapSize, irradianceMapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glViewport(0, 0, irradianceMapSize, irradianceMapSize); // don't forget to configure the viewport to the capture dimensions.

	shaderIrradiance.use();
	shaderIrradiance.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (int i = 0; i != 6; ++i)
	{
		shaderIrradiance.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Utils::renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//opengl status
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
}

void IBL_Diffuse::render(float time)
{
	glClearColor(0.1f, 0.1f, .1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderPBR.use();
	glm::mat4 model, view, projection;
	model = glm::mat4();
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(camera.Zoom), SCR_WIDTH / SCR_HEIGHT, .1f, 100.f);

	shaderPBR.setMat4("view", view);
	shaderPBR.setMat4("projection", projection);

	shaderPBR.setVec3("cameraPos", camera.Position);
	shaderPBR.setVec3("albedo", albedo);
	shaderPBR.setFloat("ao", ao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallicMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, roughnessMap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, aoMap);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
	for (unsigned int row = 0; row < nrRows; ++row)
	{
		shaderPBR.setFloat("metallic", (float)row / (float)nrRows);
		for (unsigned int col = 0; col < nrColumns; ++col)
		{
			// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			shaderPBR.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, .05f, 1.0f));
			model = glm::mat4();
			model = glm::translate(model, glm::vec3((col - ((float)nrColumns / 2)) * spacing, (row - ((float)nrRows / 2)) * spacing, 0.f));
			shaderPBR.setMat4("model", model);
			Utils::renderSphere();
		}
	}

	//在每个灯的位置渲染一个球体
	for (int i = 0; i != N_LIGHT_SIZE; ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(time * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		shaderPBR.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		shaderPBR.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		shaderPBR.setMat4("model", model);
		Utils::renderSphere();
	}

	//render skybox (render as lat to prevent overdraw)
	//默认 glDepthfunc(GL_LEQUAL)
	//因为skybox渲染的 depth都是1， 所以写入前进行比较小于之前才进行写入
	shaderSkybox.use();
	shaderSkybox.setMat4("view", view);
	glm::mat4 rotView = glm::mat4(glm::mat3(view));
	shaderSkybox.setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	Utils::renderCube();
}