#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "app.h"
#include "shader.h"
#include "Utils.h"

class coordinate : public app
{
	//����ϵͳ coordinate systems
	///Object Space	-model matrix->
	///World Space	-view matrix->
	///View Space	-projection matrix->ע:ͶӰ�任������꣬w�������ܲ�Ϊ1
	///Clip Space	-perspective divide->ע��͸�ӳ������xyz���귶Χ����(-1,1)
	///NDC space	-viewport transform->ע��ӳ�䵽��ͬ�ֱ��ʵ���Ļ
	///Screen Space	ע�����귶Χ(0,screenHeight/screenWidth)

	//ͶӰ
	///1.Orthographic projection������һ��������Ĳü��ռ�
	///ע��glm::ortho(left,right, bootom, top, near, far); ����,��/��/��/��/��/Զƽ�� ->����һ����ͶӰ����(����ı�w������ֵ)
	///2.Perspective projection,��һ��ƽ��ͷ��(��̨)�ļ��ÿռ�,�޸�ÿ������wֵ
	///ע��glm::perspective(fovy, width/height,near, far); ����,y������Ұ�Ƕ�(<)����߱ȡ�����Զƽ�� -������һ��͸��ͶӰ(��ı�w������ֵ,����۲���ԽԶ�Ķ�������w����Խ��)
	///�ο� https://www.jianshu.com/p/3a6e4f2b0f61


	///������ɫ�������Ҫ�����еĶ��㶼�ڲü��ռ���,OpenGLȻ��Բü�����ִ��͸�ӳ����Ӷ������Ǳ任����׼���豸���ꡣ
	///OpenGL��ʹ��glViewPort�ڲ��Ĳ���������׼���豸����ӳ�䵽��Ļ���꣬ÿ�����궼������һ����Ļ�ϵĵ㣨�����ǵ���������һ��800x600����Ļ����������̳�Ϊ�ӿڱ任��
	
	///OpenGL��һ����������ϵ:��x����������ֱߣ���y�ᳯ�ϣ�����z���ǳ���󷽵�

	unsigned int texture, texture1;
	unsigned int VAO;
	Shader shader;

	void start()
	{
		float vertices1[] = {
			//     ---- λ�� ----       ---- ��ɫ ----     - �������� -
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
		};

		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};


		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);

		texture = Utils::loadTexture("_texs/container.jpg");
		texture1 = Utils::loadTexture("_texs/awesomeface.png");

		shader = Shader("_shaders/_006_coordinate.vs", "_shaders/_006_coordinate.fs");
		shader.use();
		shader.setInt("ourTexture", 0);
		shader.setInt("texture1", 1);

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(.2f, .2f, .2f, 1.f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glm::mat4 model;
		model = glm::rotate(model, (float)time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		glm::mat4 view;
		// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		shader.setMat4("view", view);
		shader.use();

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);
		shader.use();

		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, (float)time * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
};