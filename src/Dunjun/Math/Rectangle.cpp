#include <Dunjun/Math/Rectangle.hpp>

#include <cmath>

namespace Dunjun
{
	Rectangle::Rectangle(f32 x, f32 y, f32 width, f32 height)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
	{

	}

	Rectangle::Rectangle(const Vector2& position, const Vector2& size)
		: x(position.x)
		, y(position.y)
		, width(size.x)
		, height(size.y)
	{

	}

	bool Rectangle::contains(const Vector2& point) const
	{
		f32 minX = std::fmin(x, x + width);
		f32 maxX = std::fmax(x, x + width);
		f32 minY = std::fmin(y, y + height);
		f32 maxY = std::fmax(y, y + height);

		return point.x >= minX && point.x < maxX && point.y >= minY && point.y < maxY;
	}

	bool Rectangle::intersects(const Rectangle& other) const
	{
		Rectangle intersection;
		return intersects(other, intersection);
	}
	
	bool Rectangle::intersects(const Rectangle& other, Rectangle& intersection) const
	{
		f32 r1MinX = std::fmin(x, x + width);
		f32 r1MaxX = std::fmax(x, x + width);
		f32 r1MinY = std::fmin(y, y + height);
		f32 r1MaxY = std::fmax(y, y + height);

		f32 r2MinX = std::fmin(other.x, other.x + other.width);
		f32 r2MaxX = std::fmax(other.x, other.x + other.width);
		f32 r2MinY = std::fmin(other.y, other.y + other.height);
		f32 r2MaxY = std::fmax(other.y, other.y + other.height);

		f32 x1 = std::fmax(r1MinX, r2MinX);
		f32 x2 = std::fmin(r1MaxX, r2MaxX);

		f32 y1 = std::fmax(r1MinY, r2MinY);
		f32 y2 = std::fmin(r1MaxY, r2MaxY);

		if (x1 < x2 && y1 < y2)
		{
			intersection = Rectangle(x1, y1, x2 - x1, y2 - y1);
			return true;
		}
		else
		{
			intersection = Rectangle(0, 0, 0, 0);
			return false;
		}
	}

	bool operator==(const Rectangle& left, const Rectangle& right)
	{
		return (left.x == right.x) && (left.y == right.y) 
			&& (left.width == right.width) && (left.height == right.height);
	}
	
	bool operator!=(const Rectangle& left, const Rectangle& right)
	{
		return !(left == right);
	}
}
