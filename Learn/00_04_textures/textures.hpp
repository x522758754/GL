#include "app.h"
#include "shader.h"
#include "Utils.h"

class textures : public app
{
	//�����Ʒ�ʽ(TEXTURE WRAP);������һ�ֵ������㳬����Χ(0, 1)ʱָ��OpenGL��β��������ģʽ��
	///GL_REPEAT			�������Ĭ����Ϊ���ظ�����
	///GL_MIRRORED_REPEAT	ÿ���ظ����Ǿ�����õ�
	///GL_CLAMP_TO_EDGE		�������걻Լ����0,1֮�䣬�����Ĳ��ֻ��ظ���������ı�Ե������һ�ֱ�Ե����Ч��
	///GL_CLAMP_TO_BORDER	����������Ϊ�û�ָ���ı�Ե��ɫ,���� glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//�������(TEXTURE FILTER):������һ�ֵ��ж�������ѡ��ʱָ��OpenGL��β��������ģʽ��
	///ע:�������겻�����ڷֱ���,��������(Texture Pixel�� Texel��
	///GL_NEAREST			Nearest Neighbor Filtering��OpenGL��ѡ�����ĵ���ӽ�����������Ǹ����أ�����״ͼ��
	///GL_LINEAR			(Bi)linear Filtering�������������긽�����������أ������һ����ֵ�����Ƴ���Щ��������֮�����ɫ��һ���������ص����ľ�����������Խ������ô����������ص���ɫ�����յ�������ɫ�Ĺ���Խ�󣻸�ƽ��ͼ��

	//Mipmap
	///GL_NEAREST_MIPMAP_NEAREST	ʹ�����ڽ��Ķ༶��Զ������ƥ�����ش�С����ʹ���ڽ���ֵ�����������
	///GL_LINEAR_MIPMAP_NEAREST	ʹ�����ڽ��Ķ༶��Զ�����𣬲�ʹ�����Բ�ֵ���в���
	///GL_NEAREST_MIPMAP_LINEAR	��������ƥ�����ش�С�Ķ༶��Զ����֮��������Բ�ֵ��ʹ���ڽ���ֵ���в���
	///GL_LINEAR_MIPMAP_LINEAR	�������ڽ��Ķ༶��Զ����֮��ʹ�����Բ�ֵ����ʹ�����Բ�ֵ���в���
	
	//��������
	///(0,1)	(0.5,1)		(1,1��
	///
	///(0,0)				(1,0)
	unsigned int texture, texture1;
	unsigned int VAO;
	Shader shader;

	void start()
	{
		float vertices[] = {
			//     ---- λ�� ----       ---- ��ɫ ----     - �������� -
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		unsigned int indices[] = { // ע��������0��ʼ! 
			0, 1, 3, // ��һ��������
			1, 2, 3  // �ڶ���������
		};

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		texture = Utils::loadTexture("_texs/container.jpg");
		texture1 = Utils::loadTexture("_texs/awesomeface.png");

		shader = Shader("_shaders/_004_textures.vs", "_shaders/_004_textures.fs");
		shader.use();
		shader.setInt("ourTexture", 0);
		shader.setInt("texture1", 1);

	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.2f, .2f, .2f, 1.f);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
};