#include "app.h"
#include "shader.h"
#include <model.h>

//模板缓冲操作
/// 允许我们在渲染片段时将模板缓冲设定为一个特定的值。通过在渲染时修改模板缓冲的内容，我们写入了模板缓冲。在同一个（或者接下来的）渲染迭代中，我们可以读取这些值，来决定丢弃还是保留某个片段。
///启用模板缓冲的写入。
///渲染物体，更新模板缓冲的内容。
///禁用模板缓冲的写入。
///渲染（其它）物体，这次根据模板缓冲的内容丢弃特定的片段。
//配置模板测试
///glStencilFunc(GL_EQUAL, 1, 0xFF); //这会告诉OpenGL，只要一个片段的模板值等于(GL_EQUAL)参考值1，片段将会通过测试并被绘制，否则会被丢弃。
///glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass) //设定模板测试结果行为：模板失败、模板成功但深度失败、模板和深度都成功
//可应用于阴影体积(Shadow Volume)的模板缓冲技术渲染实时阴影

class stencilTesting : public app
{
	Model m;
	Shader shader, shader1;

	void start()
	{
		m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_021_assimp.vs", "_shaders/_021_assimp.fs");
		shader1 = Shader("_shaders/_032_stencilTesting.vs", "_shaders/_032_stencilTesting.fs");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		glStencilMask(0x00); // 每一位在写入模板缓冲时都会变成0（禁用写入）
		glStencilMask(0xFF); // 每一位写入模板缓冲时都保持原样 默认情况

	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	//模板测试失败：保持当前存储的模板值
													//模板测试通过,但深度测试失败时：保持当前存储的模板值
													//模板测试通过,深度测试通过时：将模板值设置为glStencilFunc函数设置的ref值

		//1st: render pass, draw objects as normal, writing to stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xff); //所有的片段都能更新模板缓冲，比较:(1 & 0xff) (当前模板值 & 0xff)
		glStencilMask(0xff); //启用模板写入
		renderObj(1.0f, shader);
		//模板测试通过,深度测试通过时=>绘制过的片段模板值更新为1

		//2st: render pass: 轻微放大模型，并禁用写模板缓冲
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//片段的模板值不等于(GL_NOTEQUAL)1,片段则通过（可以绘制）
		glStencilMask(0x00);//测试通过的片段写入的为0(禁止写入)
		glDisable(GL_DEPTH_TEST);
		renderObj(1.1f, shader1);
		//=>绘制的片段模板值更新为0

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	void renderObj(float sclae, Shader shader)
	{
		shader.use();

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
																//model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(sclae, sclae, sclae));
		shader.setMat4("model", model);

		glm::mat4 view;
		// 注意，我们将矩阵向我们要进行移动场景的反方向移动。
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		m.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
																//model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(sclae, sclae, sclae));
		shader.setMat4("model", model);

		m.Draw(shader);
	}
};