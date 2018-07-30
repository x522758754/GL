#include "app.h"
#include "shader.h"
#include "Utils.h"

//��Ļ�ռ价�����ڱ�(Screen Space Ambient Occlusion)
///���������ı���(Screen-filled Quad)�ϵ�ÿһ��Ƭ�Σ����Ƕ�������ܱ����ֵ����һ���ڱ�����(Occlusion Factor)��
///����ڱ�����֮��ᱻ�������ٻ��ߵ���Ƭ�εĻ������շ�����
///�ڱ�������ͨ���ɼ�Ƭ����Χ���ͺ���(Kernel)�Ķ��������������͵�ǰƬ�����ֵ�Աȶ��õ��ġ�
///����Ƭ�����ֵ�����ĸ�������������Ҫ���ڱ����ӡ�

//����������AO��
///����ָ��ӹ���������໥�ڵ����䲻��ֵ��µ�΢����Ӱ��ͨ����ֱ�ӹ��յİ����Ż�Ƚ����ԡ�
///AO�ܸ���ʵ��ģ����ʵ���磬��ʹ�����и����Ե�����У��������Ļ�˵���ǰ����㹻��ʵ��������Ʈ��

//LightMap��������ͼ
/// https://www.zhihu.com/question/38140464

//ȫ��������GI������ָ����ֱ�ӹ�֮�������գ����֮���ӹ��յ��ܺ͡� 

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