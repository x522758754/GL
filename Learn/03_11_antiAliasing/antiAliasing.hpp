#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>

//��ݵ���Դ
///��ݵ���Դ����Ϊ�����Ķ�������ά�ռ����������ģ���������ʾ����������һ����ɢ�Ķ�ά���顣
///�����ж�һ���㵽��û�б�ĳ�����ظ��ǵ�ʱ�򵥴���һ�����С����ߡ�û��"���⣬��ʧ�������Ե���Ϣ�����¾�ݡ�

//��դ��
///��λ�����մ�����Ķ���֮��Ƭ����ɫ��֮ǰ�����������е��㷨����̵��ܺ͡�
///��դ���Ὣһ��ͼԪ�����ж�����Ϊ���룬������ת��Ϊһϵ�е�Ƭ�Ρ�
///�������������Ͽ���ȡ����ֵ����Ƭ�β��У���Ϊ�����������㴰�ڵķֱ��ʡ�
///����������Ƭ��֮�伸����ԶҲ������һ��һ��ӳ�䣬���Թ�դ��������ĳ�ַ�ʽ������ÿ�������������ڵ�Ƭ�� / ��Ļ���ꡣ

//Super Sampling AA (Super Sampling Anti Aliasing)
///����������Ļ����ķֱ�����800x600, 4xSSAA�ͻ�����Ⱦ��һ���ֱ���1600x1200��buffer�ϣ�Ȼ����ֱ�Ӱ�����Ŵ�4����buffer�²�����800x600��

//������
///һ����Ļ���ص�����ÿ�����ص����İ�����һ��������
///���ᱻ��������(Ƭ����ɫ����Ⱦ��)�������Ƿ��ڸ���ĳ�����أ������Ƿ񸲸Ǹò����㣩��
///��ÿһ����ס�����ش���������һ��Ƭ�Ρ������α�Ե��һЩ����Ҳ��ס��ĳЩ��Ļ���أ�������Щ���صĲ����㲢û�б��������ڲ����ڸǣ��������ǲ����ܵ�Ƭ����ɫ����Ӱ�졣

//���ز��� Multi-Sampling AA
///ֻ���ڹ�դ���׶�,�ִ�����GPU����Ӳ����ʵ��������㷨��������shading��������Զ���ڹ�դ���Ľ��죬�������Զ��SSAA��ܶࡣ
///���������ǽ���һ�Ĳ������Ϊ���������,���������������������ģ�����Ĳ������ܴ�������ȷ���ڸ��ʡ�
///MSAA�����Ĺ�����ʽ�ǣ������������ڸ��˶��ٸ��Ӳ����㣬��ÿ��ͼԪ�У�ÿ������ֻ����һ��Ƭ����ɫ����
///���ص���ɫ������(Ƭ����ɫ����Ⱦ��)��������ɫ���������ɫƽ��ֵ��
///һ������������и���Ĳ����㱻�������ڸǣ���ô������ص���ɫ�ͻ���ӽ��������ε���ɫ;����ÿ��������˵��Խ�ٵ��Ӳ����㱻�����������ǣ���ô���ܵ������ε�Ӱ���ԽС

//��Ⱥ�ģ����ز���
///����������ɫֵ���ܵ����ز�����Ӱ�죬��Ⱥ�ģ�����Ҳ�ܹ�ʹ�ö�������㡣
///����Ȳ�����˵��ÿ����������ֵ����������Ȳ���֮ǰ����ֵ�������������С�
///��ģ�������˵�����Ƕ�ÿ����������������ÿ�����أ��洢һ��ģ��ֵ����Ȼ����Ҳ��ζ����Ⱥ�ģ�建��Ĵ�С������Ӳ�����ĸ�����


///����ʵ������MSAA
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