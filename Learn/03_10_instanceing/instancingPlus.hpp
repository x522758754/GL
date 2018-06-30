#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
#include <string>

//��Ϊÿһ���ݽ������ɼ��������ι��ɣ���Ⱦ������˲����ɵģ�����ǧ����Ⱦ��������ȴ�Ἣ���Ӱ�����ܡ�����
///glBindVertexArray(VAO);
///glDrawArrays(GL_TRIANGLES, 0, amount_of_vertices);
///���Ƶ��ù��࣬�ﵽ����ƿ��
//����ƶ��㱾����ȣ�ʹ��glDrawArrays��glDrawElements��������GPUȥ���ƶ������ݻ����Ļ��������ܣ�
//��ΪOpenGL�ڻ��ƶ�������֮ǰ��Ҫ���ܶ�׼������(�������GPU�ô��ĸ������ȡ���ݣ�����Ѱ�Ҷ������ԣ�������Щ��������Ի�����CPU��GPU����(CPU to GPU Bus)�Ͻ��еģ���
//���ԣ�������Ⱦ����ǳ��죬����GPUȥ��Ⱦȴδ�ء�

//ʵ����:�����ܹ�������һ���Է��͸�GPU��Ȼ��ʹ��һ�����ƺ�����OpenGL������Щ���ݻ��ƶ�����壬�ͻ��������
///������ʹ��һ����Ⱦ���������ƶ�����壬����ʡÿ�λ�������ʱCPU -> GPU��ͨ�ţ���ֻ��Ҫһ�μ��ɡ�

//gl_InstanceID
///��ʹ��ʵ������Ⱦ����ʱ��gl_InstanceID���0��ʼ����ÿ��ʵ������Ⱦʱ����1��

class instancingPlus : public app
{
	GLuint VAO, VBO, instanceVBO;
	Shader shader;

	glm::vec2 translations[100];


	void start()
	{
		int index = 0;
		float offset = 0.1f;
		for (int y = -10; y < 10; y += 2)
		{
			for (int x = -10; x < 10; x += 2)
			{
				glm::vec2 translation;
				translation.x = (float)x / 10.0f + offset;
				translation.y = (float)y / 10.0f + offset;
				translations[index++] = translation;
			}
		}

		float quadVertices[] = {
			// λ��          // ��ɫ
			-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
			0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
			-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

			-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
			0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
			0.05f,  0.05f,  0.0f, 1.0f, 1.0f
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::vec2), &translations[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(2);
		/*
		OpenGL��ʲôʱ����¶������Ե���������һ�����ݡ����ĵ�һ����������Ҫ�Ķ������ԣ��ڶ������������Գ���(Attribute Divisor)��
		Ĭ������£����Գ�����0������OpenGL������Ҫ�ڶ�����ɫ����ÿ�ε���ʱ���¶������ԡ�
		��������Ϊ1ʱ�����Ǹ���OpenGL����ϣ������Ⱦһ����ʵ����ʱ����¶������ԡ�
		������Ϊ2ʱ������ϣ��ÿ2��ʵ������һ�����ԣ��Դ����ơ�
		���ǽ����Գ�������Ϊ1�����ڸ���OpenGL������λ��ֵ2�Ķ���������һ��ʵ�������顣
		*/
		glVertexAttribDivisor(2, 1);

		shader = Shader("_shaders/_0310_instancingPlus.vs", "_shaders/_0310_instancingPlus.fs");
	}

	void render(double time)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		for (unsigned int i = 0; i < 100; i++)
		{
			std::stringstream ss;
			std::string index;
			ss << i;
			index = ss.str();
			shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
			//�����������Ҫ��ȾԶ����100��ʵ����ʱ������ʵ�ǳ��ձ飩���������ջᳬ������ܹ���������ɫ����uniform���ݴ�С���ޡ�
			//����һ�����淽����ʵ��������(Instanced Array)����������Ϊһ���������ԣ��ܹ������Ǵ����������ݣ������ڶ�����ɫ����Ⱦһ���µ�ʵ��ʱ�Ż���¡�
		}
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

	}

	void shutdown()
	{

	}
};