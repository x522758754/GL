#include "app.h"
#include "shader.h"
#include "Utils.h"

//HDR（high dynamic range）
///显示器被限制为只能显示值为0.0到1.0间的颜色，但是在光照方程中却没有这个限制。
///通过使片段的颜色超过1.0，我们有了一个更大的颜色范围，这也被称作HDR(High Dynamic Range, 高动态范围)。
///有了HDR，亮的东西可以变得非常亮，暗的东西可以变得非常暗，而且充满细节。

///HDR渲染和其很相似，我们允许用更大范围的颜色值渲染从而获取大范围的黑暗与明亮的场景细节，最后将所有HDR值转换成在[0.0, 1.0]范围的LDR(Low Dynamic Range, 低动态范围)。

//色调映射（tone mapping）
///是一个损失很小的转换浮点颜色值至我们所需的LDR[0.0, 1.0]范围内的过程，通常会伴有特定的风格的色平衡(Stylistic Color Balance)。
///转换HDR值到LDR值得过程叫做色调映射(Tone Mapping)，现在现存有很多的色调映射算法，这些算法致力于在转换过程中保留尽可能多的HDR细节。
///这些色调映射算法经常会包含一个选择性倾向黑暗或者明亮区域的参数。

//注意
//标准化的定点格式
///在实现HDR渲染之前，我们首先需要一些防止颜色值在每一个片段着色器运行后被限制约束的方法。
///当帧缓冲使用了一个标准化的定点格式(像GL_RGB)为其颜色缓冲的内部格式，OpenGL会在将这些值存入帧缓冲前自动将其约束到0.0到1.0之间。
///这一操作对大部分帧缓冲格式都是成立的，除了专门用来存放被拓展范围值的浮点格式。

//浮点帧缓冲
///当一个帧缓冲的颜色缓冲的内部格式被设定成了GL_RGB16F, GL_RGBA16F, GL_RGB32F 或者GL_RGBA32F时，这些帧缓冲被叫做浮点帧缓冲(Floating Point Framebuffer)
///浮点帧缓冲可以存储超过0.0到1.0范围的浮点值，所以非常适合HDR渲染。

//hdr的作用
///1.为bloom效果提供源数据
///2.在任何有光照叠加的地方，使用HDR计算光照会更加真实、物理上也更加正确.  https://gamedev.stackexchange.com/questions/62836/does-hdr-rendering-have-any-benefits-if-bloom-wont-be-applied
///2.1 environment reflections(环境反射)
///2.2 light accumulation(多个光源)
///2.3 motion blur and depth of field(运动模糊和景深)


//hdr image
///图片在最亮和最暗之间有一个非常大的比值，hdr图片在一个正常的显示屏(低动态范围)会表现的非常平滑。 https://photo.stackexchange.com/questions/7630/what-is-tone-mapping-how-does-it-relate-to-hdr
///高亮度必须被压缩到更小的亮度范围内，使整体对比效果减弱，因此更平滑。

//tonemapping
///并不是将整个图像映射到显示器的亮度范围内，而是调整图像局部的对比度，让图像每个区域都尽可能的使用最大的对比度（如何调整则取决于使用的tonemapping算法）
///tonemapping不是hdr的原因，也可以用tonemapping使一个ldr的图像更具对比度，结果看起来会在暗处多一些噪点（noise）。

///注：hdr iamge 和 noise是对立的,事实上，in fact dynamic range is usually defined in terms of the noise floor of an image. 

class hdr : public app
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
		shaderQuad = Shader("_shaders/04_06_hdr.vs", "_shaders/04_06_hdr.fs");

		tex_diff = Utils::loadTexture("_texs/wood.png", true);

		FBO = genfboFloatingPoint(tex_hdr, cfg.scrWidth, cfg.scrHeight);

		shader.use();
		shader.setInt("texture_diffuse1", 0);

		shaderQuad.use();
		shaderQuad.setInt("screenTexture", 0);

		lightPos = glm::vec3(0, 0, 0);
		cam->set_position(glm::vec3(0, 0, 20));

		exposure = 1.0f;
		
		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		glm::mat4 model, view, projection;
		view = cam->GetViewMatrix();
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);

		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(2, 2, 50));

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("lightColor", glm::vec3(200.0, 200.0, 200.0));
		shader.setVec3("eyePos", cam->get_position());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diff);

		drawCube(&cubeVAO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		shaderQuad.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_hdr);

		shaderQuad.setFloat("exposure", exposure);


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