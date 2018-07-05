#include "app.h"
#include "shader.h"
#include "Utils.h"

//正想渲染(Forward Rendering)
///Forward Rendering渲染的复杂度可以用O(num_geometry_fragments * num_lights)来表示，可以看出复杂度和集合体的面数还有光源的数量正相关。

//延迟渲染(Deferred Rendering)
///Deferred Rendering光照渲染的时间复杂度可以用O(screen_resolution * num_lights)来表示，他和场景中物体的数量是无关的，只和光源数量有关

/// 你使用的是很多动态光源，那么就使用Deferred Rendering，但是它也有一些缺点：
/// 1.需要比较新的显卡，支持多目标渲染；
/// 2.需要很大的显卡带宽，用来传递Buffer；
/// 3.不用处理透明的对象（除非把Forward rendering 和 Deferred rendering结合起来）；
/// 4.没法用传统的抗锯齿方法， 比如MSAA，但是屏幕空间的FXAA是适用的；
/// 5.只能使用一种材质，但是有一种解决方法是Deferred Lighting；
/// 6.阴影的数量还是和光源的数量有关。

///如果游戏中没有很多的光源并且需要兼容老的设备，那么最好选择Forward Rendering，然后采用静态的Light map，效果也很不错。
///最重要的是相比正向渲染，延迟渲染大多数情况下在移动设备中性能都会比较差。因为额外的通道需要每一帧都要完成。如果只使用了一个光照，这样做可能会有点儿不值得。


class deferredShading : public app
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