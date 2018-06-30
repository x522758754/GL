#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"


class normalVisual : public app
{
	Shader shader, shaderMan;
	Model m;

	void start()
	{
		m = Model("_models/nanosuit/nanosuit.obj");
		shader = Shader("_shaders/_039_normalVisual.vs", "_shaders/_039_normalVisual.fs", "_shaders/_039_normalVisual.gs");
		shaderMan = Shader("_shaders/_021_assimp.vs", "_shaders/_021_assimp.fs");

		glEnable(GL_DEPTH_TEST);

	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glm::mat4 model, view, projection;

		model = glm::scale(model, glm::vec3(0.1f));
		view = cam->GetViewMatrix();
		projection = glm::perspective(cam->get_fovy(), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setFloat("time", time);

		m.Draw(shader);

		shaderMan.use();
		shaderMan.setMat4("model", model);
		shaderMan.setMat4("view", view);
		shaderMan.setMat4("projection", projection);

		m.Draw(shaderMan);

	}

	void shutdown()
	{
	}

};