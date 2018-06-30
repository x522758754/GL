#include "app.h"
#include "shader.h"
#include <model.h>

//深度缓冲
///和颜色缓冲一样的宽度和高度(即每个片段都有深度信息),它会以16、24或32位float的形式储存它的深度值。在大部分的系统中，深度缓冲的精度都是24位的
//深度测试
///OpenGL会将当前片段与深度缓冲(同一个坐标)的内容进行对比,OpenGL执行测试，测试通过则深度缓冲会更新为新的深度值，测试失败，则当前片段被丢弃
//执行顺序
///在片段着色器运行之后（以及模板测试之后）在屏幕空间运行的
//访问
///直接使用GLSL内建变量gl_FragCoord.z从片段着色器中直接访问。gl_FragCoord的x和y分量代表了片段的屏幕空间坐标（其中(0, 0)位于左下角)
//提前深度测试
///部分GPU的硬件特性，当使用提前深度测试时，片段着色器的一个限制是你不能写入片段的深度值。如果一个片段着色器对它的深度值进行了写入，提前深度测试是不可能的。
//深度值应在每帧渲染前清除
///如果你启用了深度缓冲，你还应该在每个渲染迭代之前使用GL_DEPTH_BUFFER_BIT来清除深度缓冲，否则你会仍在使用上一次渲染迭代中的写入的深度值：
//深度值精度
///在屏幕空间不是线性的Fdepth = (1/z - 1/near) / (1/far - 1/near) ，在观察空间是线性的 Fdepth = (z - near)/(far - near)
///透视投影(会改变w分量的值,得离观察者越远的顶点坐标w分量越大)
//深度冲突（Z-fighting）
///一个很常见的视觉错误会在两个平面或者三角形非常紧密地平行排列在一起时会发生，深度缓冲没有足够的精度来决定两个形状哪个在前面。结果就是这两个形状不断地在切换前后顺序，这会导致很奇怪的花纹
//防止深度冲突
///1.永远不要把多个物体摆得太靠近，以至于它们的一些三角形会重叠
///2.尽可能将近平面设置远一些。
///3.使用更高精度的深度缓冲
//OpenGL Projection Matrix
///投影矩阵 http://www.songho.ca/opengl/gl_projectionmatrix.html


class depthTesting : public app
{
	Shader shader;

	void start()
	{


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