#include "app.h"
#include "shader.h"
#include "Utils.h"

//屏幕空间环境光遮蔽(Screen Space Ambient Occlusion)
///对于铺屏四边形(Screen-filled Quad)上的每一个片段，我们都会根据周边深度值计算一个遮蔽因子(Occlusion Factor)。
///这个遮蔽因子之后会被用来减少或者抵消片段的环境光照分量。
///遮蔽因子是通过采集片段周围球型核心(Kernel)的多个深度样本，并和当前片段深度值对比而得到的。
///高于片段深度值样本的个数就是我们想要的遮蔽因子。

//环境闭塞（AO）
///：是指间接光在物件间相互遮挡反射不充分导致的微弱阴影。通常在直接光照的暗部才会比较明显。
///AO能更真实的模拟现实世界，能使暗部有更明显的立体感，用美术的话说就是暗部足够“实”，“不飘”

//LightMap：光照贴图
/// https://www.zhihu.com/question/38140464

//全局照明（GI）：是指除了直接光之外包括天空，物件之间间接光照的总和。 

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