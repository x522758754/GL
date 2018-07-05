#include "app.h"
#include "shader.h"
#include <model.h>
#include <vector>
#include <map>

//֡���壺��ɫ���塢��Ȼ��塢ģ�建������������Ļ���壬���洢���ڴ���

//GL_FRAMEBUFFER, GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER���ɶ���д֡���棬 ֻ����ֻд

//��ɵ�֡���棺
/// ��������һ�����壨��ɫ����Ȼ�ģ�建�壩��
/// ������һ����ɫ����(Attachment)��
/// ���еĸ����������������ģ��������ڴ棩��
/// ÿ�����嶼Ӧ������ͬ����������

//��Ⱦ������󸽼�
/// ��Ϊһ�����õ�֡���帽�����͵ģ������ڹ�ȥ������Ψһ���õĸ�����
/// ������ͼ��һ������Ⱦ���������һ�������Ļ��壬��һϵ�е��ֽڡ����������صȡ�
/// ��Ⱦ������󸽼ӵĺô��ǣ����Ὣ���ݴ���ΪOpenGLԭ������Ⱦ��ʽ������Ϊ������Ⱦ��֡�����Ż����ġ�
/// ��Ⱦ�������ͨ������ֻд�ģ������㲻�ܶ�ȡ���ǣ�����ʹ��������ʣ���
/// ��Ȼ����Ȼ�����ܹ�ʹ��glReadPixels����ȡ�������ӵ�ǰ�󶨵�֡���壬�����Ǹ��������з����ض���������ء�

//ע�⣺
///������Ⱦ�������ͨ������ֻд�ģ����ǻᾭ��������Ⱥ�ģ�帽������Ϊ�󲿷�ʱ�����Ƕ�����Ҫ����Ⱥ�ģ�建���ж�ȡֵ��ֻ������Ⱥ�ģ����ԡ�
///������Ҫ��Ⱥ�ģ��ֵ���ڲ��ԣ�������Ҫ�����ǽ��в�����������Ⱦ�������ǳ��ʺ����ǡ�
///�����ǲ���Ҫ����Щ�����в�����ʱ��ͨ������ѡ����Ⱦ���������Ϊ������Ż�һ�㡣
///���е���Ⱥ�ģ���������ӵ�ǰ�󶨵�֡�������Ⱥ�ģ�帽���У�����еĻ�����ȡ��
//������������Ȼ��壬��ô���е���Ȳ��Բ��������ٹ�������Ϊ��ǰ�󶨵�֡�����в�������Ȼ��塣��Ҫ���glEnable(GL_DEPTH_TEST) ʹ��

//������ �� ��Ⱦ������󸽼�
///ͨ���Ĺ����ǣ�����㲻��Ҫ��һ�������в������ݣ���ô���������ʹ����Ⱦ�������������ǵ�ѡ��
///�������Ҫ�ӻ����в�����ɫ�����ֵ�����ݣ���ô��Ӧ��ѡ����������
///���ܷ�������������ǳ����Ӱ��ġ�

//���Ƴ�����һ�������ϣ�������Ҫ��ȡ���µĲ��裺
///���µ�֡�����Ϊ�����֡���壬������һ����Ⱦ����
///��Ĭ�ϵ�֡����
///����һ�����������Ļ���ı��Σ���֡�������ɫ������Ϊ��������

class framebuffers : public app
{
	Model m;
	Shader shader, shaderQuad;
	GLuint texture, texture1, texColorBuffer;
	GLuint cubeVAO, planeVAO, quadVAO;
	GLuint fbo, rbo;

	void start()
	{
		//m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_034_faceCulling.vs", "_shaders/_034_faceCulling.fs");
		shaderQuad = Shader("_shaders/_035_framebuffer.vs", "_shaders/_035_framebuffer_EdgeDetection.fs");

		texture = Utils::loadTexture("_texs/marble.jpg");
		texture1 = Utils::loadTexture("_texs/metal.jpg");

		fbo = genfboColor(0, &texColorBuffer);
		rbo = genrbo();
		cubeVAO = 0, planeVAO = 0, quadVAO = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		//first pass
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		shader.use();
		shader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glm::mat4 view;
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 model;
		shader.setMat4("model", model);

		drawCube(&cubeVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		drawPlane(&planeVAO);

		//second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderQuad.use();
		shaderQuad.setInt("screenTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glBindVertexArray(quadVAO);
		drawQuad(&quadVAO);
	}
};