#ifndef DUNJUN_MATH_QUATERNION_HPP
#define DUNJUN_MATH_QUATERNION_HPP

#include <Dunjun/Math/Vector3.hpp>
#include <Dunjun/Math/Matrix4.hpp>
#include <Dunjun/Math/Angle.hpp>
#include <Dunjun/Math/Constants.hpp>

#include <ostream>

namespace Dunjun
{
struct Quaternion
{
	Quaternion();
	Quaternion(const Quaternion& q) = default;
	explicit Quaternion(f32 x, f32 y, f32 z, f32 w);
	explicit Quaternion(const Vector3& v, f32 s);

	inline const f32& operator[](usize index) const { return data[index]; }
	inline f32& operator[](usize index) { return data[index]; }

	Quaternion operator-() const;
	Quaternion operator+(const Quaternion& b) const;
	Quaternion operator-(const Quaternion& b) const;
	Quaternion operator*(const Quaternion& b) const;

	Quaternion operator*(f32 s) const;

	Quaternion operator/(f32 s) const;

	bool operator==(const Quaternion& b) const;
	bool operator!=(const Quaternion& b) const;

	const Vector3 vector() const;
	Vector3& vector();

	f32 scalar() const;
	f32& scalar();

	union
	{
		f32 data[4];
		struct { f32 x, y, z, w; };
	};
};

inline Quaternion operator*(f32 s, const Quaternion& q) { return q * s; }

f32 dot(const Quaternion& a, const Quaternion& b);
Quaternion cross(const Quaternion& a, const Quaternion& b);

f32 lengthSquared(const Quaternion& q);
f32 length(const Quaternion& q);

Quaternion normalize(const Quaternion& q);
Quaternion conjugate(const Quaternion& q);
Quaternion inverse(const Quaternion& q);

/// rotate v by q
Vector3 operator*(const Quaternion& q, const Vector3& v);

inline std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
	os << "Quaternion(";
	for (usize i = 0; i < 4; i++)
	{
		os << q[i];
		if (i < 3)
			os << ", ";
	}
	os << ")";

	return os;
}

Radian angle(const Quaternion& q);
Vector3 axis(const Quaternion& q);
Quaternion angleAxis(const Radian& angle, const Vector3& axis);

struct EulerAngles
{
	Radian pitch;
	Radian yaw;
	Radian roll;
};

Radian roll(const Quaternion& q);
Radian pitch(const Quaternion& q);
Radian yaw(const Quaternion& q);
EulerAngles quaternionToEulerAngles(const Quaternion& q);

inline Quaternion eulerAnglesToQuaternion(const EulerAngles& e, const Vector3& xAxis = { 1, 0, 0 }, const Vector3& yAxis = { 0, 1, 0 }, const Vector3& zAxis = { 0, 0, 1 });
inline Quaternion eulerAnglesToQuaternion(const Radian& pitch, const Radian& yaw, const Radian& roll, const Vector3& xAxis = { 1, 0, 0 }, const Vector3& yAxis = { 0, 1, 0 }, const Vector3& zAxis = { 0, 0, 1 })
{
	return eulerAnglesToQuaternion({ pitch, yaw, roll }, xAxis, yAxis, zAxis);
}

Matrix4 quaternionToMatrix4(const Quaternion& q);
Quaternion matrix4ToQuaternion(const Matrix4& m);
}

#endif // !DUNJUN_MATH_QUATERNION_HPP
