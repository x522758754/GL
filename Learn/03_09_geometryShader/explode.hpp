#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"


class explode : public app
{
	Shader shader;
	Model m;

	void start()
	{
		m = Model("_models/nanosuit/nanosuit.obj");
		shader = Shader("_shaders/_039_explode.vs", "_shaders/_039_explode.fs", "_shaders/_039_explode.gs");

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


	}

	void shutdown()
	{
	}

};