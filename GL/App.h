//
#ifndef __APP_H__
#define __APP_H__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include "camera.h"

class App
{
public:
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	const float SCR_WIDTH = 1280 / 1.6, SCR_HEIGHT = 720 / 1.6;
	static App* app;

	struct APPINFO
	{
		char title[128];	//标题
		int windowWidth;	//窗口宽度
		int windowHeight;	//窗口高度
		int majorVersion;	//OpenGL大版本号
		int minorVersion;	//OpenGL小版本号
		int samples;		//采样点数
		union
		{
			struct
			{
				unsigned int    fullscreen : 1;		//是否是全屏
				unsigned int    vsync : 1;			//缓冲区交换间隔(垂直同步?)		
				unsigned int    cursor : 1;			//鼠标状态
				unsigned int    stereo : 1;			//立体渲染?
				unsigned int    debug : 1;			//调试
				unsigned int    robust : 1;			//指定程序健壮性的策略
			};
			unsigned int        all;	//以上全都使用
		} flags;
	};

	virtual void run(App* the_app)
	{
		bool running = true;
		app = the_app;
		if (!glfwInit())
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			return;
		}

		init();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.minorVersion);
		glfwWindowHint(GLFW_SAMPLES, info.samples);
		glfwWindowHint(GLFW_STEREO, info.flags.stereo ? GL_TRUE : GL_FALSE);
		window = glfwCreateWindow(info.windowWidth, info.windowHeight, info.title, info.flags.fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		if (!window)
		{
			fprintf(stderr, "Failed to open window\n");
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);

		glfwSetWindowSizeCallback(window, glfw_onResize);
		glfwSetKeyCallback(window, glfw_onKey);
		glfwSetMouseButtonCallback(window, glfw_onMouseButton);
		glfwSetCursorPosCallback(window, glfw_onMouseMove);
		glfwSetScrollCallback(window, glfw_onMouseWheel);

		glfwSetInputMode(window, GLFW_CURSOR, info.flags.cursor? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			fprintf(stderr, "Failed to initialize GLAD\n");
			return;
		}
 	
		start();
		
		do 
		{
			processInput();

			render(static_cast<float>(glfwGetTime()));

			render();

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			glfwSwapBuffers(window);
			glfwPollEvents();

			running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
			running &= (glfwWindowShouldClose(window) != GL_TRUE);

		} while (running);

		shutdown();
		
		glfwDestroyWindow(window);


		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
	}

	virtual void init()
	{
		strcpy_s(info.title, "123");
		info.windowWidth = SCR_WIDTH;
		info.windowHeight = SCR_HEIGHT;
		info.majorVersion = 3;
		info.minorVersion = 3;
		info.samples = 0;
		info.flags.all = 0;
		info.flags.cursor = 0;

		camera = Camera(glm::vec3(0.f, 0.f, 3.f));
	}

	virtual void start()
	{

	}

	virtual void processInput()
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	virtual void render(float time)
	{

	}

	virtual void render()
	{

	}

	virtual void shutdown()
	{

	}

	virtual void onResize(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	virtual void onKey(int key, int action)
	{
// 		fprintf(stderr, "key %d, action %d", key, action);
// 		if (GLFW_PRESS == action || GLFW_REPEAT == action)
// 		{
// 			switch (key)
// 			{
// 			case GLFW_KEY_ESCAPE:
// 				glfwSetWindowShouldClose(window, true);
// 			case GLFW_KEY_W:
// 				camera.ProcessKeyboard(FORWARD, deltaTime);
// 			case GLFW_KEY_S:
// 				camera.ProcessKeyboard(BACKWARD, deltaTime);
// 			case GLFW_KEY_A:
// 				camera.ProcessKeyboard(LEFT, deltaTime);
// 			case GLFW_KEY_D:
// 				camera.ProcessKeyboard(RIGHT, deltaTime);
// 			default:
//				break;
// 			}
// 		}
	}

	virtual void onMouseButton(int button, int action)
	{

	}

	virtual void onMouseMove(int x, int y)
	{
		if (firstMouse)
		{
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

		lastX = x;
		lastY = y;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	virtual void onMouseWheel(int offset)
	{
		camera.ProcessMouseScroll(offset);
	}

	void getMousePosition(int& x, int& y)
	{
		double dx, dy;
		glfwGetCursorPos(window, &dx, &dy);

		x = static_cast<int>(floor(dx));
		y = static_cast<int>(floor(dy));
	}

	void setWindowTitle(const char * title)
	{
		glfwSetWindowTitle(window, title);
	}

protected:

	GLFWwindow* window;
	APPINFO info; //待测试 同时输出输出联合体全部
	
	Camera camera;
	float lastX = (float)SCR_WIDTH / 2.0;
	float lastY = (float)SCR_HEIGHT / 2.0;
	bool firstMouse = true;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	static void glfw_onResize(GLFWwindow* window, int w, int h)
	{
		app->onResize(w, h);
	}

	static void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		app->onKey(key, action);
	}

	static void glfw_onMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		app->onMouseButton(button, action);
	}

	static void glfw_onMouseMove(GLFWwindow* window, double x, double y)
	{
		app->onMouseMove(static_cast<int>(x), static_cast<int>(y));
	}

	static void glfw_onMouseWheel(GLFWwindow* window, double xoffset, double yoffset)
	{
		app->onMouseWheel(static_cast<int>(yoffset));
	}
};

#define DECLARE_GL_APP(a)	\
App * App::app = 0;			\
int main()					\
{							\
    a *app = new a;			\
    app->run(app);			\
    delete app;				\
	return 0;				\
}

#endif
