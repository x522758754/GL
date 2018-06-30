#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
//������
///����ͨ���������������������任��Ч���ϲ�����
///����������˵�˳���Ǻ���Ҫ�ģ����������˲�ͬ��������˵�˳���ǲ��ܽ����ġ�������ǽ����������˳�򽻻��Ļ����õ��Ľ���ͻ��ǣ��ȶԷɵ�����ƽ�ƣ�Ȼ����������ԭ�������ת��

///ʹ�þ���ĺô�֮һ�������ʹ��3���໥��ֱ��������ԣ����ᶨ����һ������ռ䣬���������3�������һ��ƽ������������һ�����󣬲���������������������κ�����������任���Ǹ�����ռ䡣
///��A�ռ�����������Ma,�и�����va��B�ռ��и����������Mb����������vb��va��B�ռ�ı�ʾ����vb = Mb * va

///�����ڱ�׼������ϵ����һ������v(�μ�1.2.1 ��)���������ϵ����������Ex��Ey��Ez���������⣬��������һ������ϵ�������������Fx��Fy��Fz(��Щ���������ܹ��棬Ҳ����˵��|Fx, Fy, Fz|��0)�������������ϵ,
///���ڣ������Fx��Fy��Fz��Ψ����������v ?
///	Fw = (Fx Fy Fz)w = v
///	w = F^-1 v


//������ͼ
///�����һ��ƫ��ɫ�����������������ҵ��ļ������з�����ͼ���������ģ���
///������Ϊ���з��ߵ�ָ��ƫ��z�ᣨ0, 0, 1������һ��ƫ������ɫ��
///����������z�᷽��Ҳ������������΢ƫ�ƣ���ɫҲ�ͷ�������΢�仯������������������һ����ȡ�
///ע��������ͼֻ��ģ����մ���������, �������ر��ڵ��ı���
//�Ӳ���ͼ
///�����ñ��汻�ڵ�ס�Ĳ�����ȫ����ʾ����������֮�⻹����һ����Χ�ڵ���ש�鰼͹�ĳ̶ȡ�
///�Ӳ���ͼҲֻ��ģ�����٣���û����ĸı�ģ�ͱ���

//OpenGL��ȡ�������y����V�����������ͨ���������ķ�ʽ�෴��

//���߿ռ�
///������ͼ�еķ������������߿ռ��У�������Զָ����z����
///���߿ռ���λ�������α���֮�ϵĿռ䣺��������ڵ��������εı��زο���ܡ�
///����������ͼ�����ı��ؿռ䣻���Ƕ�������Ϊָ����z�����������ձ任��ʲô����
///ʹ��һ���ض��ľ������Ǿ��ܽ�����/���߿ռ��еķ�������ת���������ͼ���꣬ʹ����ת�����յ���ͼ����ķ���

//TBN���� ��������λ��ֱ��ֻ���Ƿ���:����ռ䵽���߿ռ� (=>�����������������������߿ռ䵽����ռ䣬)
///��֪��ģ�Ϳռ䣨Object Space	���µ�tangent��normal������Matrix_model���󣨽���������任���������꣩
///��normal�任������ռ�,����Matrix_model_normal = transpose(inverse(Matrix_model))
///���world_tangent = Matrix_model * normal, world_normal = Matrix_model_normal * normal
///��ΪTBN�����˳��Ϊtangent��bitangent��normal ��Ӧx��y��z��˳�򣬿���bitangent�ķ������⣬����world_bitangent = cross(world_normal, world_tangent)
///���ˣ�����������ϵ�£���TBN�����ľ��󣬿��Խ�����������ռ�任�����߿ռ�

//TBN��������߿ռ䵽����ռ�
///tangent�����߿ռ��µ�������world������ռ��µ�����
///�裺 tangent = TBN world
///�У� TBN^-1 tangent = TBN ^-1 TBN world 
///		TBN^-1 tangent = world

//view���� (right�� up�� front):����ռ䵽����ռ� (wc:�������������ϵ������,�����Ҫ��ƽ�Ƶ�ԭ��)
///															rx	ux	fx	0			1	0	0	-wcx
///Matrix_lookAt =	 Matrix_rotation * Matrix_translate = [	ry	uy	fy	0	] * [	0	1	0	-wcy	]
///															rz	uz	fz	0			0	0	1	-wcz
///															rw	uw	fw	1			0	0	0	w
///Matrix_translate���Ƚ����������ռ�����ϵ�µ�λ���Ƶ�ԭ�㣨ע��Ŀ���������Ϊԭ����н���������ת���ο�real-time�鼮��
///Matrix_rotation: ���������������ϵ��right�� up�� front�����ľ��󣻿��Խ�����������ռ�任������ռ�

class normalMapping : public app
{
	Shader shader;
	GLuint tex_diff, tex_normal, tex_spec;
	GLuint planeVAO, cubeVAO;
	glm::vec3 lightPos;

	Model m;

	void start()
	{	
		shader = Shader("_shaders/_0404_normalMapping.vs", "_shaders/_0404_normalMapping.fs");

		tex_diff = Utils::loadTexture("_texs/brickwall.jpg");
		tex_normal = Utils::loadTexture("_texs/brickwall_normal.jpg");

		m = Model("_models/Cyborg/Cyborg.obj");

		shader.use();
		shader.setInt("texture_diffuse1", 0);
		shader.setInt("texture_normal1", 1);

		cam->set_position(glm::vec3(0, 0, 3));
		lightPos = glm::vec3(2, 2, 2);
		planeVAO = 0;

		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		glm::mat4 model, view, projection;
		view = cam->GetViewMatrix();
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", cam->get_position());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diff);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_normal);

		drawPlane();

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::translate(model, glm::vec3(0.1f));
		shader.setMat4("model", model);

		drawCube(&cubeVAO);

		model = glm::mat4();
		shader.setMat4("model", model);

		m.Draw(shader);



	}

	void drawPlane()
	{
		if (planeVAO == 0)
		{
			// positions
			glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
			glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
			glm::vec3 pos3(1.0f, -1.0f, 0.0f);
			glm::vec3 pos4(1.0f, 1.0f, 0.0f);

			// texture coordinates
			glm::vec2 uv1(0.0f, 1.0f);
			glm::vec2 uv2(0.0f, 0.0f);
			glm::vec2 uv3(1.0f, 0.0f);
			glm::vec2 uv4(1.0f, 1.0f);

			// normal vector
			glm::vec3 nm(0.0f, 0.0f, 1.0f);

			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1;
			glm::vec3 tangent2;
			// triangle 1
			// ----------
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			// triangle 2
			// ----------
			edge1 = pos3 - pos1;
			edge2 = pos4 - pos1;
			deltaUV1 = uv3 - uv1;
			deltaUV2 = uv4 - uv1;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent2 = glm::normalize(tangent2);

			float vertice[] = {
				// positions            // normal         // texcoords  // tangent                          // bitangent
				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, 
				pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, 
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, 

				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, 
				pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, 
			};

			glGenVertexArrays(1, &planeVAO);
			glBindVertexArray(planeVAO);

			GLuint VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(3);
		}

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};