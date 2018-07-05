#include "app.h"
#include "shader.h"
#include "Utils.h"

//屏幕空间环境光遮蔽(Screen Space Ambient Occlusion)
///对于铺屏四边形(Screen-filled Quad)上的每一个片段，我们都会根据周边深度值计算一个遮蔽因子(Occlusion Factor)。
///这个遮蔽因子之后会被用来减少或者抵消片段的环境光照分量。
///遮蔽因子是通过采集片段周围球型核心(Kernel)的多个深度样本，并和当前片段深度值对比而得到的。
///高于片段深度值样本的个数就是我们想要的遮蔽因子。
class ssao : public app
{
	Shader shader, shaderQuad;
	GLuint tex_hdr, tex_diff, tex_normal, tex_disp;
	GLuint cubeVAO, quadVAO;
	GLuint FBO;
	glm::vec3 lightPos;
	float exposure;

	void start()
	{
		shader = Shader("_shaders/04_06_light.vs", "_shaders/04_06_light.fs");
		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		glm::mat4 model, view, projection;
		view = cam->GetViewMatrix();
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);
	}

};