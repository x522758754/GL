#include "app.h"
#include "shader.h"
#include "Utils.h"

//HDR��high dynamic range��
///��ʾ��������Ϊֻ����ʾֵΪ0.0��1.0�����ɫ�������ڹ��շ�����ȴû��������ơ�
///ͨ��ʹƬ�ε���ɫ����1.0����������һ���������ɫ��Χ����Ҳ������HDR(High Dynamic Range, �߶�̬��Χ)��
///����HDR�����Ķ������Ա�÷ǳ��������Ķ������Ա�÷ǳ��������ҳ���ϸ�ڡ�

///HDR��Ⱦ��������ƣ����������ø���Χ����ɫֵ��Ⱦ�Ӷ���ȡ��Χ�ĺڰ��������ĳ���ϸ�ڣ��������HDRֵת������[0.0, 1.0]��Χ��LDR(Low Dynamic Range, �Ͷ�̬��Χ)��

//ɫ��ӳ�䣨tone mapping��
///��һ����ʧ��С��ת��������ɫֵ�����������LDR[0.0, 1.0]��Χ�ڵĹ��̣�ͨ��������ض��ķ���ɫƽ��(Stylistic Color Balance)��
///ת��HDRֵ��LDRֵ�ù��̽���ɫ��ӳ��(Tone Mapping)�������ִ��кܶ��ɫ��ӳ���㷨����Щ�㷨��������ת�������б��������ܶ��HDRϸ�ڡ�
///��Щɫ��ӳ���㷨���������һ��ѡ��������ڰ�������������Ĳ�����

//ע��
//��׼���Ķ����ʽ
///��ʵ��HDR��Ⱦ֮ǰ������������ҪһЩ��ֹ��ɫֵ��ÿһ��Ƭ����ɫ�����к�����Լ���ķ�����
///��֡����ʹ����һ����׼���Ķ����ʽ(��GL_RGB)Ϊ����ɫ������ڲ���ʽ��OpenGL���ڽ���Щֵ����֡����ǰ�Զ�����Լ����0.0��1.0֮�䡣
///��һ�����Դ󲿷�֡�����ʽ���ǳ����ģ�����ר��������ű���չ��Χֵ�ĸ����ʽ��

//����֡����
///��һ��֡�������ɫ������ڲ���ʽ���趨����GL_RGB16F, GL_RGBA16F, GL_RGB32F ����GL_RGBA32Fʱ����Щ֡���屻��������֡����(Floating Point Framebuffer)
///����֡������Դ洢����0.0��1.0��Χ�ĸ���ֵ�����Էǳ��ʺ�HDR��Ⱦ��

//hdr������
///1.ΪbloomЧ���ṩԴ����
///2.���κ��й��յ��ӵĵط���ʹ��HDR������ջ������ʵ��������Ҳ������ȷ.  https://gamedev.stackexchange.com/questions/62836/does-hdr-rendering-have-any-benefits-if-bloom-wont-be-applied
///2.1 environment reflections(��������)
///2.2 light accumulation(�����Դ)
///2.3 motion blur and depth of field(�˶�ģ���;���)


//hdr image
///ͼƬ���������֮����һ���ǳ���ı�ֵ��hdrͼƬ��һ����������ʾ��(�Ͷ�̬��Χ)����ֵķǳ�ƽ���� https://photo.stackexchange.com/questions/7630/what-is-tone-mapping-how-does-it-relate-to-hdr
///�����ȱ��뱻ѹ������С�����ȷ�Χ�ڣ�ʹ����Ա�Ч����������˸�ƽ����

//tonemapping
///�����ǽ�����ͼ��ӳ�䵽��ʾ�������ȷ�Χ�ڣ����ǵ���ͼ��ֲ��ĶԱȶȣ���ͼ��ÿ�����򶼾����ܵ�ʹ�����ĶԱȶȣ���ε�����ȡ����ʹ�õ�tonemapping�㷨��
///tonemapping����hdr��ԭ��Ҳ������tonemappingʹһ��ldr��ͼ����߶Աȶȣ�������������ڰ�����һЩ��㣨noise����

///ע��hdr iamge �� noise�Ƕ�����,��ʵ�ϣ�in fact dynamic range is usually defined in terms of the noise floor of an image. 

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