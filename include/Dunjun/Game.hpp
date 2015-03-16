#ifndef DUNJUN_GAME_HPP
#define DUNJUN_GAME_HPP

#include <Dunjun/Math.hpp>

#include <Dunjun/OpenGL.hpp>
#include <GLFW/glfw3.h>

namespace Dunjun
{
namespace Game
{
	void init();
	void run();
	void cleanup();

	GLFWwindow* getGlfwWindow();
	Vector2 getWindowSize();
} //namespace Game
} //namespace Dunjun

#endif // !DUNJUN_GAME_HPP
