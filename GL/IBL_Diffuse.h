//疑问，tbn矩阵的构造、球体的参数方程、球体的构造
#pragma once
#include "App.h"
#include <Utils.h>
#include <shader.h>

class IBL_Diffuse :public App
{
	const int N_LIGHT_SIZE = 4;
	const int nrRows = 7, nrColumns = 7;
	const int envCubemapSize = 512, irradianceMapSize = 32;
	Shader shaderPBR, shaderMap, shaderSkybox, shaderIrradiance;
	std::vector<glm::vec3> lightPositions, lightColors;
	glm::vec3 albedo = glm::vec3(0.5f, 0.0f, 0.0f);
	float ao = 1.f; //ambient occlusion
	float spacing = 2.5; //每个球的间隔

	unsigned int albedoMap, normalMap, metallicMap, roughnessMap, aoMap, equirectangularMap;
	unsigned int captureFBO, captureRBO, envCubemap, irradianceFBO, irradianceMap;

	glm::mat4 captureProjection;
	std::vector<glm::mat4> captureViews;
	void start();
	
	void render(float time);
};