#include "app.h"
#include "shader.h"
#include <model.h>
#include <vector>
#include <map>


class environmentMapping : public app
{
	Model m;
	Shader shader, shaderCube;
	GLuint texture, textureCube;
	GLuint VAO, cubeVAO;
	GLuint fbo, rbo;

	void start()
	{
		VAO = 0, cubeVAO = 0;
		std::vector<std::string> faces
		{
			"_texs/mp_drakeq/drakeq_ft.tga",
			"_texs/mp_drakeq/drakeq_bk.tga",

			"_texs/mp_drakeq/drakeq_up.tga",
			"_texs/mp_drakeq/drakeq_dn.tga",

			"_texs/mp_drakeq/drakeq_rt.tga",
			"_texs/mp_drakeq/drakeq_lf.tga",
		};

		//m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_036_cubemaps.vs", "_shaders/_036_cubemaps.fs");
		shaderCube = Shader("_shaders/_036_environment.vs", "_shaders/_036_environment.fs");

		texture = Utils::loadCubemap(faces);
		textureCube = Utils::loadTexture("_texs/marble.jpg");

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glm::mat4 model, view, projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		view = cam->GetViewMatrix();

		shaderCube.use();
		shaderCube.setInt("texture_diffuse1", 0);
		shaderCube.setInt("skybox", 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCube);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		shaderCube.setVec3("cameraPos", cam->get_position());

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.2f));
		shaderCube.setMat4("model", model);
		shaderCube.setMat4("view", view);
		shaderCube.setMat4("projection", projection);
		drawCube(&cubeVAO);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

		shader.use();
		shader.setInt("cubemap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		model = glm::scale(model, glm::vec3(1000));
		shader.setMat4("model", model);
		drawSkybox(&VAO);

		glDepthFunc(GL_LESS); // set depth function back to default
	}

	void shutdown()
	{
		//optional: de - allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &VAO);
		// 		glDeleteBuffers(1, &cubeVBO);
		// 		glDeleteBuffers(1, &skyboxVAO);
	}
};