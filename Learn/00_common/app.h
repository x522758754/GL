#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"

struct appConfig
{
	/// ���ڱ���
	char title[256];
	
	/// ���ڿ��
	int scrWidth;

	/// ���ڸ߶�
	int scrHeight;

	/// OpengGL ���汾��
	int majorVersion;

	/// OpenGL �ΰ汾��
	int minorVersion;

	/// ÿ�������ϵĲ�������Ŀ,ֵ����Ӱ��ͼ�λ�������
	int samples;
};

class app
{
public:
	app();
	
	virtual ~app();

	///��ʼ����Ϣ
	virtual void init();

	virtual void start();

	virtual void run();

	virtual void render(double time);

	virtual void shutdown();

	virtual void onResize(int w, int h);

	virtual void onInput();

	virtual void onKey(int key, int action);

	virtual void onMouseClick(int button, int action);

	virtual void onMouseMove(int x, int y);

	virtual void onWheel(int offset);

	void getMousePos(int &x, int &y);

	glm::mat4 getProjM();

	static app* _app;
protected:
	GLFWwindow* window;
	appConfig cfg;

	/// ���
	camera *cam;
	/// ÿ֡���ʱ��
	float deltaTime;
	/// ��һ֡��ʱ���
	float lastFrameTime;
	/// ���λ�õ�
	glm::vec2 mousePos;
	/// ��һ����Ӧ���
	bool bFirstMove;
	/// ����ƶ�����
	bool bMoveDisable;
	/// �����ʾ����
	bool bCursorDisable;

private:
	const float SCR_WIDTH = 1280.f, SCR_HEIGHT = 720.f;

	static void glfw_onResize(GLFWwindow* window, int w, int h)
	{
		_app->onResize(w, h);
	}

	static void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		_app->onKey(key, action);
	}

	static void glfw_onMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		_app->onMouseClick(button, action);
	}

	static void glfw_onMouseMove(GLFWwindow* window, double x, double y)
	{
		_app->onMouseMove(x, y);
	}

	static void glfw_onMouseWheel(GLFWwindow* window, double xoffset, double yoffset)
	{
		_app->onWheel(yoffset);
	}
};

#define  DECLARE_APP(a)	\
int main()				\
{						\
	a* _a = new a;		\
	_a->run();			\
	delete _a;			\
	return 0;			\
}