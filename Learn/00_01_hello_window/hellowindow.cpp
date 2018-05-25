#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);

int main1()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed!" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed" << std::endl;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	while (!glfwWindowShouldClose(window)) //��ⴰ���Ƿ񱻹ر�
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT); //ָ��Ҫ����Ļ���������

		glfwSwapBuffers(window);//����˫���������(��ÿ�����ص���ɫ��
		glfwPollEvents(); //�Ƿ����¼���������������ƶ����������룩
	}

	glfwTerminate(); //�ͷ��ѷ������Դ
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}