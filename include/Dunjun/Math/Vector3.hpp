#ifndef DUNJUN_MATH_VECTOR3_HPP
#define DUNJUN_MATH_VECTOR3_HPP

#include <Dunjun/Types.hpp>
#include <Dunjun/Math/Vector2.hpp>

#include <cmath>

namespace Dunjun
{
struct Vector3
{
	Vector3()
		: x(0)
		, y(0)
		, z(0)
	{

	}
	explicit Vector3(f32 xyz)
		: x(xyz)
		, y(xyz)
		, z(xyz)
	{

	}
		Vector3(f32 x, f32 y, f32 z)
		: x(x)
		, y(y)
		, z(z)
	{

	}
	Vector3(f32 xyz[3])
		: x(xyz[0])
		, y(xyz[1])
		, z(xyz[2])
	{

	}

	explicit Vector3(const Vector2& other, f32 z)
		: Vector3(other.x, other.y, z)
	{

	}

	f32& operator[](usize index) { return data[index]; }
	const f32& operator[](usize index) const { return data[index]; }

	Vector3 operator+(const Vector3& other) const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator-(const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 operator*(f32 scalar) const
	{
		return Vector3(scalar * x, scalar * y, scalar * z);
	}

	Vector3 operator/(f32 scalar) const
	{
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	Vector3& operator+=(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	Vector3& operator-=(const Vector3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	Vector3& operator*=(f32 scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	Vector3& operator/=(f32 scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	union
	{
		f32 data[3];
		struct
		{
			f32 x, y, z;
		};
		struct
		{
			f32 r, g, b;
		};
		struct
		{
			f32 s, t, p;
		};
	};
};

inline Vector3 operator*(f32 scalar, const Vector3& vector) { return vector * scalar; }

inline f32 dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline Vector3 cross(const Vector3& a, const Vector3& b) { return Vector3(a.y * b.z - b.y * a.z, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

inline f32 length(const Vector3& a) { return std::sqrtf(dot(a, a)); }

inline f32 lengthSquared(const Vector3& a) { return lengthSquared(a); }

inline Vector3 normalized(const Vector3& a) { return a * (1.0 / length(a)); }
} //namespace Dunjun


#endif
