#include <Dunjun/Input.hpp>

#include <Dunjun/Game.hpp>

namespace Dunjun
{
	namespace Input
	{
		KeyState getKey(Key key)
		{
			return static_cast<KeyState>(glfwGetKey(Game::getGlfwWindow(), key));
		}

		void setInputMode(InputMode mode, int value)
		{
			int m = 0;
			if (mode == InputMode::Cursor)
				m = GLFW_CURSOR;
			if (mode == InputMode::StickyKeys)
				m = GLFW_STICKY_KEYS;
			if (mode == InputMode::StickyMouseButtons)
				m = GLFW_STICKY_MOUSE_BUTTONS;

			glfwSetInputMode(Game::getGlfwWindow(), m, value);
		}

		Vector2 getCursorPosition()
		{
			f64 x, y;
			glfwGetCursorPos(Game::getGlfwWindow(), &x, &y);
			return Vector2(x, y);
		}

		void setCursorPosition(const Vector2& position)
		{
			glfwSetCursorPos(Game::getGlfwWindow(), static_cast<f64>(position.x), static_cast<f64>(position.y));
		}

	} // namespace Input
} // namespace Dunjun