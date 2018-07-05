#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>


//点光阴影(point shadows)
///本节我们的焦点是在各种方向生成动态阴影。这个技术可以适用于点光源，生成所有方向上的阴影。

//生成深度立方体贴图
///为创建一个光周围的深度值的立方体贴图，我们必须渲染场景6次：每次一个面。显然渲染场景6次需要6个不同的视图矩阵，每次把一个不同的立方体贴图面附加到帧缓冲对象上。
///这会很耗费性能因为一个深度贴图下需要进行很多渲染调用。这个教程中我们将转而使用另外的一个小技巧来做这件事，几何着色器允许我们使用一次渲染过程来建立深度立方体贴图。

#define depthMapWidth 1024
#define depthMapHeight 1024

class pointShadows : public app
{
	GLuint cubeVAO, planeVAO, quadVAO;
	Shader shader, shadowShader;
	GLuint depthMap, texture_diffuse1, texture_specular1, depthCubemap;
	GLuint depthMapFBO;
	glm::vec3 lightPos;

	bool shadows = true;


	void start()
	{
		lightPos = glm::vec3(0.f); //针对renderscene()

		cam->set_position(lightPos);

		shader = Shader("_shaders/_0403_pointShadows_depth.vs", "_shaders/_0403_pointShadows_depth.fs", "_shaders/_0403_pointShadows_depth.gs");
		shadowShader = Shader("_shaders/_0403_pointShadows_blinnPhong.vs", "_shaders/_0403_pointShadows_blinnPhong.fs");

		texture_diffuse1 = Utils::loadTexture("_texs/wood.png");
		texture_specular1 = Utils::loadTexture("_texs/wood.png");

		depthMapFBO = genfboDepthCube(&depthCubemap, depthMapWidth, depthMapHeight);

		//当生成一个深度立方体贴图时我们只关心深度值，
		//所以我们必须显式告诉OpenGL这个帧缓冲对象不会渲染到一个颜色缓冲里。
		///帧缓冲对象不是完全不包含颜色缓冲的
		///我们需要显示告诉OpenGL我们不进行任何颜色数据进行渲染
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		//lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;

		float near_plane = 1.0f;
		float far_plane = 100.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)depthMapWidth / (float)depthMapHeight, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1.首先渲染场景的深度贴图

		///需要改变视口的参数以适应depthMap的尺寸
		///如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
		glViewport(0, 0, depthMapWidth, depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4();
		shader.use();
		shader.setMat4("model", model);
		shader.setFloat("far_plane", far_plane);
		shader.setVec3("lightPos", lightPos);
		for (unsigned int i = 0; i < 6; ++i)
			shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

		renderScene(shader);

		// 2.渲染深度贴图到屏幕
		glViewport(0, 0, cfg.scrWidth, cfg.scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(cam->get_fovy()), (float)cfg.scrWidth / (float)cfg.scrHeight, 0.1f, 100.0f);
		glm::mat4 view = cam->GetViewMatrix();

		shadowShader.use();
		shadowShader.setInt("depthMap", 0);
		shadowShader.setInt("texture_diffuse1", 1);
		shadowShader.setInt("texture_specular1", 2);
		shadowShader.setVec3("lightPos", lightPos);
		shadowShader.setVec3("viewPos", cam->get_position());
		shadowShader.setFloat("far_plane", far_plane);
		shadowShader.setBool("shadows", shadows);
		shadowShader.setMat4("projection", projection);
		shadowShader.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture_specular1);

		renderScene(shadowShader);
	}


	void renderScene(const Shader &shader)
	{
		// room cube
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(25.0f));
		shader.setMat4("model", model);
		glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
		shader.setBool("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
		drawCube(&cubeVAO);
		shader.setBool("reverse_normals", 0); // and of course disable it
		//glEnable(GL_CULL_FACE);
		// cubes
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.75f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.75f));
		shader.setMat4("model", model);
		drawCube(&cubeVAO);
	}

	void shutdown()
	{

	}

	void onKey(int key, int action)
	{
		app::onKey(key, action);
		if (action)
		{
			switch (key)
			{
			case GLFW_KEY_SPACE:
				shadows = !shadows;
				break;
			}
		}
	}

};