#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>

//锯齿的来源
///锯齿的来源是因为场景的定义在三维空间中是连续的，而最终显示的像素则是一个离散的二维数组。
///所以判断一个点到底没有被某个像素覆盖的时候单纯是一个“有”或者“没有"问题，丢失了连续性的信息，导致锯齿。

//光栅化
///是位于最终处理过的顶点之后到片段着色器之前所经过的所有的算法与过程的总和。
///光栅化会将一个图元的所有顶点作为输入，并将它转换为一系列的片段。
///顶点坐标理论上可以取任意值，但片段不行，因为它们受限于你窗口的分辨率。
///顶点坐标与片段之间几乎永远也不会有一对一的映射，所以光栅化必须以某种方式来决定每个顶点最终所在的片段 / 屏幕坐标。

//Super Sampling AA (Super Sampling Anti Aliasing)
///假设最终屏幕输出的分辨率是800x600, 4xSSAA就会先渲染到一个分辨率1600x1200的buffer上，然后再直接把这个放大4倍的buffer下采样致800x600。

//单采样
///一个屏幕像素的网格，每个像素的中心包含有一个采样点
///它会被用来决定(片段着色器渲染的)三角形是否遮盖了某个像素（根据是否覆盖该采样点）。
///在每一个遮住的像素处都会生成一个片段。三角形边缘的一些部分也遮住了某些屏幕像素，但是这些像素的采样点并没有被三角形内部所遮盖，所以它们不会受到片段着色器的影响。

//多重采样 Multi-Sampling AA
///只是在光栅化阶段,现代所有GPU都在硬件上实现了这个算法，而且在shading的运算量远大于光栅化的今天，这个方法远比SSAA快很多。
///所做的正是将单一的采样点变为多个采样点,采样点的数量可以是任意的，更多的采样点能带来更精确的遮盖率。
///MSAA真正的工作方式是，无论三角形遮盖了多少个子采样点，（每个图元中）每个像素只运行一次片段着色器。
///像素的颜色将会是(片段着色器渲染的)三角形颜色与采样点颜色平均值。
///一个像素中如果有更多的采样点被三角形遮盖，那么这个像素的颜色就会更接近于三角形的颜色;对于每个像素来说，越少的子采样点被三角形所覆盖，那么它受到三角形的影响就越小

//深度和模板多重采样
///不仅仅是颜色值会受到多重采样的影响，深度和模板测试也能够使用多个采样点。
///对深度测试来说，每个顶点的深度值会在运行深度测试之前被插值到各个子样本中。
///对模板测试来说，我们对每个子样本，而不是每个像素，存储一个模板值。当然，这也意味着深度和模板缓冲的大小会乘以子采样点的个数。


///本节实现离屏MSAA
#define SAMPLES 4

class antiAliasing : public app
{
	GLuint cubeVAO, quadVAO;
	Shader shader, shaderScreen;

	GLuint textureMS, textureIntermediate;
	GLuint msFBO, intermediateFBO;

	void start()
	{
		shader = Shader("_shaders/_011_colors.vs", "_shaders/_011_colors.fs");
		shaderScreen = Shader("_shaders/_035_framebuffer.vs", "_shaders/_035_framebuffer.fs");
		
		msFBO = genfboMS(SAMPLES, &textureMS);

		intermediateFBO = genfboColor(0, &textureIntermediate);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, msFBO);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glm::mat4 model, view, projection;
		model = glm::mat4();
		view = cam->GetViewMatrix();
		projection = glm::perspective(cam->get_fovy(), 1.f * cfg.scrWidth / cfg.scrHeight, .1f, 100.f);

		shader.use();
		shader.setVec3("objectColor", glm::vec3(1.f, .5f, .31f));
		shader.setVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		drawCube(&cubeVAO);

		// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, cfg.scrWidth, cfg.scrHeight, 0, 0, cfg.scrWidth, cfg.scrHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		shaderScreen.use();
		shaderScreen.setInt("screenTexture", 0);
		shaderScreen.setInt("samples", SAMPLES);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIntermediate);

		drawQuad(&quadVAO);
	}

	void shutdown()
	{

	}
};