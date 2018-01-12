//疑问，tbn矩阵的构造、球体的参数方程、球体的构造
#pragma once
#include "App.h"
#include <Utils.h>
#include <shader.h>

class PBR :public App
{
	const int N_LIGHT_SIZE = 4;
	const int nrRows = 7, nrColumns = 7;
	Shader shaderPBR;
	std::vector<glm::vec3> lightPositions, lightColors;
	glm::vec3 albedo = glm::vec3(0.5f, 0.0f, 0.0f);
	float ao = 1.f; //ambient occlusion
	float spacing = 2.5; //每个球的间隔

	unsigned int albedoMap, normalMap, metallicMap, roughnessMap, aoMap;

	void start()
	{
		//shader
		shaderPBR = Shader("shaders/pbrModel.vs", "shaders/pbrModel.fs");
		
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

		shaderPBR.use();
		shaderPBR.setInt("albedoMap", 0);
		shaderPBR.setInt("normalMap", 1);
		shaderPBR.setInt("metallicMap", 2);
		shaderPBR.setInt("roughnessMap", 3);
		shaderPBR.setInt("aoMap", 4);

		//opengl status
		glEnable(GL_DEPTH_TEST);
	}

	void render(float time)
	{
		glClearColor(0.1f, 0.1f, .1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shaderPBR.use();
		glm::mat4 model, view, projection;
		model = glm::mat4();
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(45.f), SCR_WIDTH / SCR_HEIGHT, .1f, 100.f);

		shaderPBR.setMat4("view", view);
		shaderPBR.setMat4("projection", projection);

		shaderPBR.setVec3("cameraPos", camera.Position);
		shaderPBR.setVec3("albedo", albedo);
		shaderPBR.setFloat("ao", ao);

		// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
		for (unsigned int row =0; row < nrRows; ++row)
		{
			shaderPBR.setFloat("metallic", (float)row / (float)nrRows);
			for (unsigned int col =0; col < nrColumns; ++ col)
			{
				// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				shaderPBR.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, .05f, 1.0f));
				model = glm::mat4();
				model = glm::translate(model, glm::vec3((col - ((float)nrColumns / 2)) * spacing, (row - ((float)nrRows / 2)) * spacing, 0.f));
				shaderPBR.setMat4("model", model);
				renderSphere();
			}
		}

		for (int i=0; i != N_LIGHT_SIZE; ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(time * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			shaderPBR.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			shaderPBR.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4();
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			shaderPBR.setMat4("model", model);
			renderSphere();
		}

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
	}

	// renders (and builds at first invocation) a sphere
	// -------------------------------------------------
	//备注：不明白如何绘制的球体方法（UVSphere方法、IcoSpher方法） 
	//不明白球体的参数方程如何推理的
	// x = r·sin(α)·cos(β)
	// y = r·cos(α) 
	// z = r·sin(α)·sin(β)
	// r 是球的半径，α角是半径与Y轴正向的夹角，β角是半径在xz平面的投影与x轴正向的夹角，他们的取值范围是
	// 0≤r≤∞    0≤α≤π    0≤β≤2π
	unsigned int sphereVAO = 0;
	unsigned int indexCount;
	void renderSphere()
	{
		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = 128;
			const unsigned int Y_SEGMENTS = 128;
			const float PI = 3.14159265359;
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y       * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y       * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}
			indexCount = indices.size();

			std::vector<float> data;
			for (int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);

				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
			}
			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			float stride = (3 + 2 + 3) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}
};