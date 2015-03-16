#ifndef DUNJUN_INPUT_HPP
#define DUNJUN_INPUT_HPP

#include <Dunjun/Common.hpp>

#include <Dunjun/OpenGL.hpp>
#include <GLFW/glfw3.h>

#include <Dunjun/Math.hpp>

namespace Dunjun
{
namespace Input
{
	using Key = int;
	using MouseButton = int;

	enum KeyState : u32
	{
		Release = 0,
		Press = 1,
	};

	enum class InputMode : u32
	{
		Cursor,
		StickyKeys,
		StickyMouseButtons,
	};

	KeyState getKey(Key key);
	void setInputMode(InputMode mode, int value);

	Vector2 getCursorPosition();
	void setCursorPosition(const Vector2& position);

} // namespace Input
} // namespace Dunjun

#endif // !DUNJUN_INPUT_HPP
