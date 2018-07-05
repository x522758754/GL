#include "app.h"
#include "shader.h"
#include "Utils.h"

//����(DOF/depth-of-field)
///����������ģ���Ŀռ䷶Χ,���ݿռ䷶Χ��С����Ϊǳ����ռ䷶ΧС�������ռ䷶Χ��
///���ƾ�����Ԍ������P������Ҫ�������s�y�ľ���Ƚo��������ͻ�@���w��
///���㾰�����й�ʽ��
/// https://www.zhihu.com/question/22430557
///����:��Ƭ���ĵ�����ľ���
///��ƽ��:�ڽ��㸽���ĳ���ƽ��,ʹ�ó�������
///�Ͻ�ƽ��:������������ƽ��
///��ɢԲ: �Ͻ��������һ���㣬���ڽ�ƽ����Ҳ��һ���㡣��δ�Ͻ������أ�����һ�������ڽ�ƽ������һ��Բ�����ң�����Ͻ�ƽ��ԽԶ�����壬����ʱ�γɵ�Բ��Խ��

//����(heat haze)

//ë����/ģ������(fuzzy glass)

//��˹ģ��(Gaussian blur��
///http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
///ʹ��˫����������˼����������
///Convolution filter: An algorithm that combines the color value of a group of pixels.
///NxN-tap filter: A filter that uses a square shaped footprint of pixels with the square��s side length being N pixels.
///N-tap filter �C A filter that uses an N-pixel footprint. Note that an N-tap filter does *not* necessarily mean that the filter has to sample N texels as we will see that an N-tap filter can be implemented using less than N texel fetches.
///Filter kernel �C A collection of relative coordinates and weights that are used to combine the pixel footprint of the filter.
///Discrete sampling �C Texture sampling method when we fetch the data of exactly one texel (aka GL_NEAREST filtering).
///Linear sampling �C Texture sampling method when we fetch a footprint of 2��2 texels and we apply a bilinear filter to aquire the final color information (aka GL_LINEAR filtering).

class bloom : public app
{
	Shader shaderLighting, shaderLight, shaderBlur, shaderBloom;
	GLuint tex_hdr[2], tex_pingpong[2], tex_diff, tex_diff1, tex_normal, tex_disp;
	GLuint cubeVAO, quadVAO;
	GLuint FBO, pingpongFBO[2];
	glm::vec3 lightPos;
	float exposure;

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;


	void start()
	{
		shaderLighting = Shader("_shaders/04_07_lighting.vs", "_shaders/04_07_lighting.fs");
		shaderLight = Shader("_shaders/04_07_light.vs", "_shaders/04_07_light.fs");
		shaderBlur = Shader("_shaders/04_07_blur.vs", "_shaders/04_07_blur.fs");
		shaderBloom = Shader("_shaders/04_07_bloom.vs", "_shaders/04_07_bloom.fs");

		tex_diff = Utils::loadTexture("_texs/wood.png", true);
		tex_diff1 = Utils::loadTexture("_texs/container2.png", true);

		FBO = genfboFloatingPointMultiColor(tex_hdr, cfg.scrWidth, cfg.scrHeight, 2);

		pingpongFBO[0] = genfboFloatingPoint(tex_pingpong[0], cfg.scrWidth, cfg.scrHeight);
		pingpongFBO[1] = genfboFloatingPoint(tex_pingpong[1], cfg.scrWidth, cfg.scrHeight);

		shaderLighting.use();
		shaderLighting.setInt("texture_diffuse1", 0);

		shaderLight.use();
		shaderLight.setInt("screenTexture", 0);

		shaderBlur.use();
		shaderBlur.setInt("image", 0);

		shaderBloom.use();
		shaderBloom.setInt("screenTexture", 0);
		shaderBloom.setInt("bloomBlur", 1);

		lightPos = glm::vec3(0, 0, 0);
		cam->set_position(glm::vec3(0, 0, 20));

		exposure = 1.0f;

		// positions
		lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
		lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
		lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
		lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
		// colors
		lightColors.push_back(glm::vec3(2.0f, 2.0f, 2.0f));
		lightColors.push_back(glm::vec3(1.5f, 0.0f, 0.0f));
		lightColors.push_back(glm::vec3(0.0f, 0.0f, 1.5f));
		lightColors.push_back(glm::vec3(0.0f, 1.5f, 0.0f));

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.f);


		glm::mat4 model, view, projection;
		view = cam->GetViewMatrix();
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);

		glEnable(GL_DEPTH_TEST);

		//render scene into floating point framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.f);

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(50, 5, 50));

		shaderLighting.use();
		shaderLighting.setMat4("view", view);
		shaderLighting.setMat4("projection", projection);
		shaderLighting.setVec3("eyePos", cam->get_position());

		for (int i=0; i != lightPositions.size(); ++i)
		{
			shaderLighting.setVec3("lights[" + std::to_string(i) + "].position", lightPositions[i]);
			shaderLighting.setVec3("lights[" + std::to_string(i) + "].color", lightColors[i]);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diff);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		glBindTexture(GL_TEXTURE_2D, tex_diff1);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
		model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(1.25));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
		model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shaderLighting.setMat4("model", model);
		drawCube(&cubeVAO);

		shaderLight.use();
		shaderLight.setMat4("projection", projection);
		shaderLight.setMat4("view", view);

		for (int i = 0; i != lightPositions.size(); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.25f));
			shaderLight.setMat4("model", model);
			shaderLight.setVec3("lightColor", lightColors[i]);
			drawCube(&cubeVAO);
		}

		//blur bright fragments with two-pass Gaussian blur
		bool horizontal = true, firstBlur = true;
		shaderBlur.use();

		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i != 10; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, horizontal ? pingpongFBO[0] : pingpongFBO[1]);

			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.1f, 0.1f, 0.1f, 1.f);

			glBindTexture(GL_TEXTURE_2D, firstBlur ? tex_hdr[1] : tex_pingpong[horizontal]);
			shaderBlur.setBool("horizontal", horizontal);
			drawQuad(&quadVAO);
			horizontal = !horizontal;
			firstBlur = false;
		}

		//now render floating point color buffer to 2D quad and tonemap hdr colors to defualt framebuffer's (clamped) color range
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		//glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		shaderBloom.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_hdr[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_pingpong[1]);

		shaderBloom.setFloat("exposure", exposure);
		shaderBloom.setBool("bloom", true);

		drawQuad(&quadVAO);
	}

	void onKey(int key, int action)
	{
		app::onKey(key, action);

		if (action)
		{
			switch (key)
			{
			case GLFW_KEY_1:
				exposure = 0.1f; break;
			case GLFW_KEY_2:
				exposure = 1.0f; break;
			case GLFW_KEY_3:
				exposure = 5.0f; break;
			default:
				break;
			}
		}
	}
};