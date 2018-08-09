#include "app.h"
#include "shader.h"
#include  <direct.h>  
//1.vertex shader
//2.primitive assembly
//3.geometry shader
//4.rasterization stage
// normalized device coordinates,NDC,����������vertex shader����֮�����Ǿ�Ӧ����ndc��,����xyz ��(-1.0,1.0)
//ͨ��glViewport��������Viewport Transform, ndc => screen-space coordinates

/// �����������Vertex Array Object��
/// ���㻺�����Vertex Buffer Object
/// �����������Element Buffer Object��EBO��Index Buffer Object��IBO

///��⣺VAO ָ����ǰ���Ƶ��ĸ�ģ�ͣ�VAO �����񶥵㻺������������󶨣��κ����Ķ������Ե��ö��ᴢ�������VAO�С�
///		 ���ڲ�ͬ��ģ�ͣ���ͬ�Ķ������ԣ����߲�ͬ����������(��һ����VBO)��ֻ��Ҫ�󶨲�ͬ��VAO��
///		 VBO������VAO�������������ģ�͵Ķ������ݣ� ��GPU�ϴ����ڴ�洢�������ݣ�ʹ�û������ĺô��������ǿ���һ���Եķ���һ�������ݵ��Կ��ϣ�������ÿ�����㷢��һ�Ρ�
///																										��CPU�����ݷ��͵��Կ���Խ���������ֻҪ�������Ƕ�Ҫ���Ծ���һ���Է��;����ܶ�����ݡ�
///																										�����ݷ������Կ����ڴ��к󣬶�����ɫ���������������ʶ��㣬���Ǹ��ǳ���Ĺ��̡�

///glDrawArrays�����ƺ�������ʹ�õ�ǰ�������ɫ����֮ǰ�Ķ��嶥���������ú�VBO�Ķ������ݣ�ͨ��VAO��󶨣�������ͼԪ��

class helloTriangle : public app
{
	///���������������VBO��OpenGL�ĺ���ģʽҪ������ʹ��VAO,
	///�洢�������Ͷ�������״̬��
	///���Ӷ�������
	unsigned int vertexArrayObject;

	Shader shader;

	void start()
	{
		// 		char   buffer[MAX_PATH];
		// 		_getcwd(buffer, MAX_PATH);
		// 		printf("The   current   directory   is:   %s ", buffer);

		float vertices[9] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};

		float vertices1[] = {
			0.5f, 0.5f, 0.0f,   // ���Ͻ�
			0.5f, -0.5f, 0.0f,  // ���½�
			-0.5f, -0.5f, 0.0f, // ���½�
			-0.5f, 0.5f, 0.0f   // ���Ͻ�
		};

		//1
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		//2
		///���㻺�����,����GPU�ڴ�(�Դ�)�д洢�Ķ�������,ʹ�û������ĺô�����һ���Է���һ�����ݵ�GPU
		///��CPU�����ݴ��ڴ淢�͵��Դ���,֮��vertex shader�������������ʶ���
		unsigned int vertexBufferObject;
		glGenBuffers(1, &vertexBufferObject); //����1���������
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		///GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
		///GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
		///GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
		///�����ε�λ�����ݲ���ı䣬ÿ����Ⱦ����ʱ������ԭ������������ʹ�����������GL_STATIC_DRAW�������
		///����˵һ�������е����ݽ�Ƶ�����ı䣬��ôʹ�õ����;���GL_DYNAMIC_DRAW��GL_STREAM_DRAW����������ȷ���Կ������ݷ����ܹ�����д����ڴ沿�֡�
		//��GPU�н����ڴ���䣬�洢����
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

		//3
		unsigned int indices[] = { // ע��������0��ʼ! 
			0, 1, 3, // ��һ��������
			1, 2, 3  // �ڶ���������
		};
		///һ���洢����������������ʹ�õĻ������
		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//4
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shader = Shader("_shaders/_002_helloTriangle.vs", "_shaders/_002_helloTriangle.fs");
		shader.use();
	}


	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.2f, .2f, .2f, 1.f);

 		glBindVertexArray(vertexArrayObject);
// 		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
};