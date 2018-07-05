#include "app.h"
#include "shader.h"
#include <model.h>
#include <vector>
#include <map>

//帧缓冲：颜色缓冲、深度缓冲、模板缓冲结合起来的屏幕缓冲，被存储在内存中

//GL_FRAMEBUFFER, GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER，可读可写帧缓存， 只读，只写

//完成得帧缓存：
/// 附加至少一个缓冲（颜色、深度或模板缓冲）。
/// 至少有一个颜色附件(Attachment)。
/// 所有的附件都必须是完整的（保留了内存）。
/// 每个缓冲都应该有相同的样本数。

//渲染缓冲对象附件
/// 作为一个可用的帧缓冲附件类型的，所以在过去纹理是唯一可用的附件。
/// 和纹理图像一样，渲染缓冲对象是一个真正的缓冲，即一系列的字节、整数、像素等。
/// 渲染缓冲对象附加的好处是，它会将数据储存为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过的。
/// 渲染缓冲对象通常都是只写的，所以你不能读取它们（比如使用纹理访问）。
/// 当然你仍然还是能够使用glReadPixels来读取它，这会从当前绑定的帧缓冲，而不是附件本身，中返回特定区域的像素。

//注意：
///由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，只关心深度和模板测试。
///我们需要深度和模板值用于测试，但不需要对它们进行采样，所以渲染缓冲对象非常适合它们。
///当我们不需要从这些缓冲中采样的时候，通常都会选择渲染缓冲对象，因为它会更优化一点。
///所有的深度和模板操作都会从当前绑定的帧缓冲的深度和模板附件中（如果有的话）读取。
//如果你忽略了深度缓冲，那么所有的深度测试操作将不再工作，因为当前绑定的帧缓冲中不存在深度缓冲。需要配合glEnable(GL_DEPTH_TEST) 使用

//纹理附件 和 渲染缓冲对象附件
///通常的规则是，如果你不需要从一个缓冲中采样数据，那么对这个缓冲使用渲染缓冲对象会是明智的选择。
///如果你需要从缓冲中采样颜色或深度值等数据，那么你应该选择纹理附件。
///性能方面它不会产生非常大的影响的。

//绘制场景到一个纹理上，我们需要采取以下的步骤：
///将新的帧缓冲绑定为激活的帧缓冲，和往常一样渲染场景
///绑定默认的帧缓冲
///绘制一个横跨整个屏幕的四边形，将帧缓冲的颜色缓冲作为它的纹理。

class framebuffers : public app
{
	Model m;
	Shader shader, shaderQuad;
	GLuint texture, texture1, texColorBuffer;
	GLuint cubeVAO, planeVAO, quadVAO;
	GLuint fbo, rbo;

	void start()
	{
		//m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_034_faceCulling.vs", "_shaders/_034_faceCulling.fs");
		shaderQuad = Shader("_shaders/_035_framebuffer.vs", "_shaders/_035_framebuffer_EdgeDetection.fs");

		texture = Utils::loadTexture("_texs/marble.jpg");
		texture1 = Utils::loadTexture("_texs/metal.jpg");

		fbo = genfboColor(0, &texColorBuffer);
		rbo = genrbo();
		cubeVAO = 0, planeVAO = 0, quadVAO = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		//first pass
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		shader.use();
		shader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glm::mat4 view;
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 model;
		shader.setMat4("model", model);

		drawCube(&cubeVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		drawPlane(&planeVAO);

		//second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderQuad.use();
		shaderQuad.setInt("screenTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glBindVertexArray(quadVAO);
		drawQuad(&quadVAO);
	}
};