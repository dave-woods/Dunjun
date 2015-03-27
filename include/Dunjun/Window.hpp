#ifndef DUNJUN_WINDOW_HPP
#define DUNJUN_WINDOW_HPP

#include <Dunjun/Math.hpp>
#include <Dunjun/OpenGL.hpp>
#include <GLFW/glfw3.h>

namespace Dunjun
{
namespace Window
{
	extern GLFWwindow* ptr;
	extern bool isFullscreen;


	GLFWwindow* createWindow(GLFWmonitor* monitor);
	void destroyWindow();
	void destroyWindow(GLFWwindow* windowPtr);

	void makeContextCurrent();
	void swapInterval(int i);
	bool shouldClose();

	void swapBuffers();
	void pollEvents();

	bool init();
	void cleanup();

	void setTitle(const char* title);

	Vector2 getWindowSize();
	Vector2 getFramebufferSize();

	bool isInFocus();
	bool isIconified();
}
}


#endif // !DUNJUN_WINDOW_HPP
