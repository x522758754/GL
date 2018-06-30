#include "app.h"
#include "shader.h"
#include <model.h>

//��Ȼ���
///����ɫ����һ���Ŀ�Ⱥ͸߶�(��ÿ��Ƭ�ζ��������Ϣ),������16��24��32λfloat����ʽ�����������ֵ���ڴ󲿷ֵ�ϵͳ�У���Ȼ���ľ��ȶ���24λ��
//��Ȳ���
///OpenGL�Ὣ��ǰƬ������Ȼ���(ͬһ������)�����ݽ��жԱ�,OpenGLִ�в��ԣ�����ͨ������Ȼ�������Ϊ�µ����ֵ������ʧ�ܣ���ǰƬ�α�����
//ִ��˳��
///��Ƭ����ɫ������֮���Լ�ģ�����֮������Ļ�ռ����е�
//����
///ֱ��ʹ��GLSL�ڽ�����gl_FragCoord.z��Ƭ����ɫ����ֱ�ӷ��ʡ�gl_FragCoord��x��y����������Ƭ�ε���Ļ�ռ����꣨����(0, 0)λ�����½�)
//��ǰ��Ȳ���
///����GPU��Ӳ�����ԣ���ʹ����ǰ��Ȳ���ʱ��Ƭ����ɫ����һ���������㲻��д��Ƭ�ε����ֵ�����һ��Ƭ����ɫ�����������ֵ������д�룬��ǰ��Ȳ����ǲ����ܵġ�
//���ֵӦ��ÿ֡��Ⱦǰ���
///�������������Ȼ��壬�㻹Ӧ����ÿ����Ⱦ����֮ǰʹ��GL_DEPTH_BUFFER_BIT�������Ȼ��壬�����������ʹ����һ����Ⱦ�����е�д������ֵ��
//���ֵ����
///����Ļ�ռ䲻�����Ե�Fdepth = (1/z - 1/near) / (1/far - 1/near) ���ڹ۲�ռ������Ե� Fdepth = (z - near)/(far - near)
///͸��ͶӰ(��ı�w������ֵ,����۲���ԽԶ�Ķ�������w����Խ��)
//��ȳ�ͻ��Z-fighting��
///һ���ܳ������Ӿ������������ƽ����������ηǳ����ܵ�ƽ��������һ��ʱ�ᷢ������Ȼ���û���㹻�ľ���������������״�ĸ���ǰ�档���������������״���ϵ����л�ǰ��˳����ᵼ�º���ֵĻ���
//��ֹ��ȳ�ͻ
///1.��Զ��Ҫ�Ѷ������ڵ�̫���������������ǵ�һЩ�����λ��ص�
///2.�����ܽ���ƽ������ԶһЩ��
///3.ʹ�ø��߾��ȵ���Ȼ���
//OpenGL Projection Matrix
///ͶӰ���� http://www.songho.ca/opengl/gl_projectionmatrix.html


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
		// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		m.Draw(shader);
	}
};