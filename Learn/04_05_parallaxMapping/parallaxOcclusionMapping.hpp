#include "app.h"
#include "shader.h"
#include "Utils.h"

///视差遮蔽映射(Parallax Occlusion Mapping)和陡峭视差映射的原则相同，但不是用触碰的第一个深度层的纹理坐标，而是在触碰之前和之后，在深度层之间进行线性插值。
///我们根据表面的高度距离啷个深度层的深度层值的距离来确定线性插值的大小。
///参考陡峭视差映射

class parallaxOcclusionMapping : public app
{
	Shader shader, shaderNoraml;
	GLuint tex_diff, tex_normal, tex_disp;
	GLuint planeVAO, cubeVAO;
	glm::vec3 lightPos;

	void start()
	{
		shader = Shader("_shaders/_0405_parallaxOcclusionMapping.vs", "_shaders/_0405_parallaxOcclusionMapping.fs");
		shaderNoraml = Shader("_shaders/_0404_normalMapping.vs", "_shaders/_0404_normalMapping.fs");

		tex_diff = Utils::loadTexture("_texs/bricks2.jpg");
		tex_normal = Utils::loadTexture("_texs/bricks2_normal.jpg");
		tex_disp = Utils::loadTexture("_texs/bricks2_disp.jpg");

		shader.use();
		shader.setInt("texture_diffuse1", 0);
		shader.setInt("texture_normal1", 1);
		shader.setInt("texture_height1", 2);

		shaderNoraml.use();
		shaderNoraml.setInt("texture_diffuse1", 0);
		shaderNoraml.setInt("texture_normal1", 1);

		cam->set_position(glm::vec3(0, 0, 3));
		lightPos = glm::vec3(2, 2, 2);
		planeVAO = 0;

		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		glm::mat4 model, view, projection;
		view = cam->GetViewMatrix();
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", cam->get_position());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diff);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex_disp);

		drawPlane();

		shaderNoraml.use();
		shaderNoraml.setMat4("model", model);
		shaderNoraml.setMat4("view", view);
		shaderNoraml.setMat4("projection", projection);
		shaderNoraml.setVec3("lightPos", lightPos);
		shaderNoraml.setVec3("viewPos", cam->get_position());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diff);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_normal);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-2.2, 0, 0));
		shaderNoraml.setMat4("model", model);

		drawPlane();


		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::translate(model, glm::vec3(0.1f));
		shaderNoraml.setMat4("model", model);

		drawCube(&cubeVAO);
	}

	void drawPlane()
	{
		if (planeVAO == 0)
		{
			// positions
			glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
			glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
			glm::vec3 pos3(1.0f, -1.0f, 0.0f);
			glm::vec3 pos4(1.0f, 1.0f, 0.0f);

			// texture coordinates
			glm::vec2 uv1(0.0f, 1.0f);
			glm::vec2 uv2(0.0f, 0.0f);
			glm::vec2 uv3(1.0f, 0.0f);
			glm::vec2 uv4(1.0f, 1.0f);

			// normal vector
			glm::vec3 nm(0.0f, 0.0f, 1.0f);

			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1;
			glm::vec3 tangent2;
			// triangle 1
			// ----------
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			// triangle 2
			// ----------
			edge1 = pos3 - pos1;
			edge2 = pos4 - pos1;
			deltaUV1 = uv3 - uv1;
			deltaUV2 = uv4 - uv1;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent2 = glm::normalize(tangent2);

			float vertice[] = {
				// positions            // normal         // texcoords  // tangent                          // bitangent
				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
				pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,

				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z,
				pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z,
			};

			glGenVertexArrays(1, &planeVAO);
			glBindVertexArray(planeVAO);

			GLuint VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(3);
		}

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};