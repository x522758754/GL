#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"

struct appConfig
{
	/// 窗口标题
	char title[256];
	
	/// 窗口宽度
	int scrWidth;

	/// 窗口高度
	int scrHeight;

	/// OpengGL 主版本号
	int majorVersion;

	/// OpenGL 次版本号
	int minorVersion;

	/// 每个像素上的采样点数目,值过大影响图形绘制性能
	int samples;
};

class app
{
public:
	app();
	
	virtual ~app();

	///初始化信息
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

	/// 相机
	camera *cam;
	/// 每帧间隔时间
	float deltaTime;
	/// 上一帧的时间点
	float lastFrameTime;
	/// 鼠标位置点
	glm::vec2 mousePos;
	/// 第一次响应鼠标
	bool bFirstMove;
	/// 鼠标移动禁用
	bool bMoveDisable;
	/// 鼠标显示禁用
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