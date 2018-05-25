#include "app.h"
#include <iostream>

app* app::_app = 0;
app::app() :
	deltaTime(0),
	lastFrameTime(0),
	bFirstMove(true),
	bCursorDisable(false),
	bMoveDisable(false),
	mousePos(0, 0)
{
	cam = new camera(glm::vec3(0.f, 0.f, 3.f));
}

app::~app()
{

}

void app::init()
{
	memset(&cfg, 0, sizeof(cfg));
	strcpy(cfg.title, "");
	cfg.scrWidth		= 1280.f;
	cfg.scrHeight		= 720.f;
	cfg.samples			= 1;
	cfg.majorVersion	= 3;
	cfg.minorVersion	= 3;

	
}

void app::start()
{

}

void app::run()
{
	bool running = true;

	_app = this;

	if (!glfwInit())
	{
		std::cerr << "glfwInit Failed!" << std::endl;
		return;
	}

	init();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.minorVersion);
	glfwWindowHint(GLFW_SAMPLES, cfg.samples);
	window = glfwCreateWindow(cfg.scrWidth, cfg.scrHeight, cfg.title, NULL, NULL);
	if (!window)
	{
		std::cerr << "glfwCreateWindow Failed!" << std::endl;
	}
	glfwMakeContextCurrent(window);

	//GLFW_CURSOR_DISABLED，隐藏光标，光标焦点始终在窗口上
	///GLFW_CURSOR_NORMAL
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetWindowSizeCallback(window, glfw_onResize);
	glfwSetKeyCallback(window, glfw_onKey);
	glfwSetMouseButtonCallback(window, glfw_onMouseButton);
	glfwSetCursorPosCallback(window, glfw_onMouseMove);
	glfwSetScrollCallback(window, glfw_onMouseWheel);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "gladLoadGLLoader Failed!" << std::endl;
	}

	start();

	do 
	{
		deltaTime = glfwGetTime() - lastFrameTime;
		lastFrameTime = glfwGetTime();

		onInput();

		render(glfwGetTime());

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwWindowShouldClose(window))
			break;

	} while (running);
}

void app::render(double time)
{

}

void app::shutdown()
{
}

void app::onResize(int w, int h)
{
	glViewport(0, 0, w, h);
}

void app::onInput()
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->handleKey(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->handleKey(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->handleKey(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->handleKey(RIGHT, deltaTime);
}

void app::onKey(int key, int action)
{
	if (action)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);break;
		case GLFW_KEY_LEFT:
			cam->handleEular(Left); break;
		case GLFW_KEY_RIGHT:
			cam->handleEular(Right); break;
		case GLFW_KEY_DOWN:
			cam->handleEular(Down); break;
		case GLFW_KEY_UP:
			cam->handleEular(Up); break;
		default:
			break;
		}
	}
}

void app::onMouseClick(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		bCursorDisable = !bCursorDisable;
		int cursorStatus = bCursorDisable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
		glfwSetInputMode(window, GLFW_CURSOR, cursorStatus);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		bMoveDisable = !bMoveDisable;
		if (bMoveDisable) bFirstMove = true; //重置第一次鼠标移动
	}
}

void app::onMouseMove(int x, int y)
{
	if (bMoveDisable) return;

	if (bFirstMove)
	{
		mousePos = glm::vec2(x, y);
		bFirstMove = false;
	}
	float xoffset = x - mousePos.x;
	float yoffset = mousePos.y - y;// reversed since y-coordinates go from bottom to top

	mousePos = glm::vec2(x, y);

	cam->handleMouse(xoffset, yoffset);
}

void app::onWheel(int offset)
{
	cam->handleWheel(offset);
}

void app::getMousePos(int &x, int &y)
{
	double dx, dy;
	glfwGetCursorPos(window, &dx, &dy);

	x = dx;
	y = dy;
}

glm::mat4 app::getProjM()
{
	return glm::perspective(glm::radians(45.0f), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
}
