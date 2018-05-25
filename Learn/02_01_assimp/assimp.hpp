#include "app.h"
#include "shader.h"
#include <model.h>

///模型顶点数据 position, normal, texcoord, binormal, color,

///https://www.jianshu.com/p/f7f3e7b6ebf5
///.obj文件： Geometric vertex, Parameter space vertices, Face elements, Vertex indices,
///			optionally (Vertex texture coordinate indices, Vertex normal indices, Vertex normal indices without texture coordinate indices, Line elements)

class assimp : public app
{
	Model m;
	Shader shader;

	void start()
	{
		m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_021_assimp.vs", "_shaders/_021_assimp.fs");
		shader.use();

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		//model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		shader.setMat4("model", model);

		glm::mat4 view;
		// 注意，我们将矩阵向我们要进行移动场景的反方向移动。
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		m.Draw(shader);
	}
};