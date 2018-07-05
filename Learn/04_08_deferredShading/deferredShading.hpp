#include "app.h"
#include "shader.h"
#include "Utils.h"

//������Ⱦ(Forward Rendering)
///Forward Rendering��Ⱦ�ĸ��Ӷȿ�����O(num_geometry_fragments * num_lights)����ʾ�����Կ������ӶȺͼ�������������й�Դ����������ء�

//�ӳ���Ⱦ(Deferred Rendering)
///Deferred Rendering������Ⱦ��ʱ�临�Ӷȿ�����O(screen_resolution * num_lights)����ʾ�����ͳ�����������������޹صģ�ֻ�͹�Դ�����й�

/// ��ʹ�õ��Ǻܶද̬��Դ����ô��ʹ��Deferred Rendering��������Ҳ��һЩȱ�㣺
/// 1.��Ҫ�Ƚ��µ��Կ���֧�ֶ�Ŀ����Ⱦ��
/// 2.��Ҫ�ܴ���Կ�������������Buffer��
/// 3.���ô���͸���Ķ��󣨳��ǰ�Forward rendering �� Deferred rendering�����������
/// 4.û���ô�ͳ�Ŀ���ݷ����� ����MSAA��������Ļ�ռ��FXAA�����õģ�
/// 5.ֻ��ʹ��һ�ֲ��ʣ�������һ�ֽ��������Deferred Lighting��
/// 6.��Ӱ���������Ǻ͹�Դ�������йء�

///�����Ϸ��û�кܶ�Ĺ�Դ������Ҫ�����ϵ��豸����ô���ѡ��Forward Rendering��Ȼ����þ�̬��Light map��Ч��Ҳ�ܲ���
///����Ҫ�������������Ⱦ���ӳ���Ⱦ�������������ƶ��豸�����ܶ���Ƚϲ��Ϊ�����ͨ����Ҫÿһ֡��Ҫ��ɡ����ֻʹ����һ�����գ����������ܻ��е����ֵ�á�


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