#include "app.h"
#include "shader.h"
#include "Utils.h"

///陡峭视差映射(Steep Parallax Mapping)是视差映射的扩展，原则是一样的，但不是使用一个样本而是多个样本来确定偏移向量的大小
///注意：陡峭视差贴图同样有自己的问题。因为这个技术是基于有限的样本数量的，我们会遇到锯齿效果以及图层之间有明显的断层
///		 从性能上来说消耗是很大的，所有的操作都是像素级别的，并且其中包含大量的纹理采样，根据需要使用。

///1.将高度[0,1]分n层,每层高度为1/n,初始高度为0，遍历从高度0开始
///2.相机的在切线空间观察方向viewDir、人为偏移缩放参数和层数n =》 每次uv偏移参数
///3.当前纹理坐标texCoords =》当前纹理检索的高度图代表纹理偏移的近似值height
///4.判断当前检索的层数所在的高度是否小于（当前纹理偏移后所对应的高度）
///5.是的话，增加高度，偏移纹理，检索当前纹理对应的高度
///6,否的话，返回当前纹理即高度图近似纹理


class stepParallaxMapping : public app
{
	Shader shader, shaderNoraml;
	GLuint tex_diff, tex_normal, tex_disp;
	GLuint planeVAO, cubeVAO;
	glm::vec3 lightPos;

	void start()
	{
		shader = Shader("_shaders/_0405_stepParallaxMapping.vs", "_shaders/_0405_stepParallaxMapping.fs");
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