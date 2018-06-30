#include "app.h"
#include "shader.h"
#include <model.h>

//ģ�建�����
/// ������������ȾƬ��ʱ��ģ�建���趨Ϊһ���ض���ֵ��ͨ������Ⱦʱ�޸�ģ�建������ݣ�����д����ģ�建�塣��ͬһ�������߽������ģ���Ⱦ�����У����ǿ��Զ�ȡ��Щֵ���������������Ǳ���ĳ��Ƭ�Ρ�
///����ģ�建���д�롣
///��Ⱦ���壬����ģ�建������ݡ�
///����ģ�建���д�롣
///��Ⱦ�����������壬��θ���ģ�建������ݶ����ض���Ƭ�Ρ�
//����ģ�����
///glStencilFunc(GL_EQUAL, 1, 0xFF); //������OpenGL��ֻҪһ��Ƭ�ε�ģ��ֵ����(GL_EQUAL)�ο�ֵ1��Ƭ�ν���ͨ�����Բ������ƣ�����ᱻ������
///glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass) //�趨ģ����Խ����Ϊ��ģ��ʧ�ܡ�ģ��ɹ������ʧ�ܡ�ģ�����ȶ��ɹ�
//��Ӧ������Ӱ���(Shadow Volume)��ģ�建�弼����Ⱦʵʱ��Ӱ

class stencilTesting : public app
{
	Model m;
	Shader shader, shader1;

	void start()
	{
		m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_021_assimp.vs", "_shaders/_021_assimp.fs");
		shader1 = Shader("_shaders/_032_stencilTesting.vs", "_shaders/_032_stencilTesting.fs");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		glStencilMask(0x00); // ÿһλ��д��ģ�建��ʱ������0������д�룩
		glStencilMask(0xFF); // ÿһλд��ģ�建��ʱ������ԭ�� Ĭ�����

	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	//ģ�����ʧ�ܣ����ֵ�ǰ�洢��ģ��ֵ
													//ģ�����ͨ��,����Ȳ���ʧ��ʱ�����ֵ�ǰ�洢��ģ��ֵ
													//ģ�����ͨ��,��Ȳ���ͨ��ʱ����ģ��ֵ����ΪglStencilFunc�������õ�refֵ

		//1st: render pass, draw objects as normal, writing to stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xff); //���е�Ƭ�ζ��ܸ���ģ�建�壬�Ƚ�:(1 & 0xff) (��ǰģ��ֵ & 0xff)
		glStencilMask(0xff); //����ģ��д��
		renderObj(1.0f, shader);
		//ģ�����ͨ��,��Ȳ���ͨ��ʱ=>���ƹ���Ƭ��ģ��ֵ����Ϊ1

		//2st: render pass: ��΢�Ŵ�ģ�ͣ�������дģ�建��
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//Ƭ�ε�ģ��ֵ������(GL_NOTEQUAL)1,Ƭ����ͨ�������Ի��ƣ�
		glStencilMask(0x00);//����ͨ����Ƭ��д���Ϊ0(��ֹд��)
		glDisable(GL_DEPTH_TEST);
		renderObj(1.1f, shader1);
		//=>���Ƶ�Ƭ��ģ��ֵ����Ϊ0

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	void renderObj(float sclae, Shader shader)
	{
		shader.use();

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
																//model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(sclae, sclae, sclae));
		shader.setMat4("model", model);

		glm::mat4 view;
		// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		m.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
																//model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(sclae, sclae, sclae));
		shader.setMat4("model", model);

		m.Draw(shader);
	}
};