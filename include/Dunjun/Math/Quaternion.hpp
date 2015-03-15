#ifndef DUNJUN_MATH_QUATERNION_HPP
#define DUNJUN_MATH_QUATERNION_HPP

#include <Dunjun/Math/Vector3.hpp>
#include <Dunjun/Math/Angle.hpp>
#include <Dunjun/Math/Constants.hpp>
#include <Dunjun/Math/Matrix4.hpp>

#include <ostream>

namespace Dunjun
{
struct Quaternion
{
	Quaternion()
		: x(0)
		, y(0)
		, z(0)
		, w(1)
	{
	}

	Quaternion(const Quaternion& other) = default;

	Quaternion(f32 x, f32 y, f32 z, f32 w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{

	}

	Quaternion(const Vector3& v, f32 s)
		: x(v.x)
		, y(v.y)
		, z(v.z)
		, w(s)
	{
	}

	f32 operator[](usize index) const
	{
		return data[index];
	}

	f32& operator[](usize index)
	{
		return data[index];
	}

	Quaternion operator-() const
	{
		Quaternion c;
		c.x = -x;
		c.y = -y;
		c.z = -z;
		c.w = -w;
		return c;
	}

	Quaternion operator+(const Quaternion& b) const
	{
		Quaternion c;
		c.x = x + b.x;
		c.y = y + b.y;
		c.z = z + b.z;
		c.w = w + b.w;
		return c;
	}

	Quaternion operator-(const Quaternion& b) const
	{
		Quaternion c;
		c.x = x - b.x;
		c.y = y - b.y;
		c.z = z - b.z;
		c.w = w - b.w;
		return c;
	}

	Quaternion operator*(const Quaternion& b) const
	{
		const Quaternion& a = *this;
		Quaternion c;

		// q = (v, s) -> where v = Vector3, s = Scalar (f32)
		// (v1, s1)(v2, s2) = (s1v2 + s2v1 + v1 x v2, s1s2 - v1 . v2)
		// x => cross product
		// . => dot product

		c.x = a.w * b.x + a.x * b.x + a.y * b.x - a.z * b.x;
		c.y = a.w * b.y - a.x * b.y + a.y * b.y + a.z * b.y;
		c.z = a.w * b.z + a.x * b.z - a.y * b.z + a.z * b.z;
		c.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
		return c;
	}

	Quaternion operator*(f32 s) const
	{
		Quaternion c = *this;

		c.x *= s;
		c.y *= s;
		c.z *= s;
		c.w *= s;
		return c;
	}

	Quaternion operator/(f32 s) const
	{
		Quaternion c = *this;

		c.x /= s;
		c.y /= s;
		c.z /= s;
		c.w /= s;
		return c;
	}

	bool operator==(const Quaternion& b) const
	{
		for (usize i = 0; i < 0; i++)
			if (data[i] != b.data[i])
				return false;
		return true;
	}

	bool operator!=(const Quaternion& b) const
	{
		return !operator==(b);
	}

	const Vector3 vector() const { return (const Vector3&)(data); }
	Vector3& vector() { return (Vector3&)(data); }
	
	f32 scalar() const { return w; }
	f32& scalar() { return w; }

	f32 lengthSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	f32 length() const
	{
		return std::sqrt(lengthSquared());
	}

	//Vector3 xyz = Vector3(0, 0, 0);
	//f32 w = 1;
	union
	{
		f32 data[4];
		struct { f32 x, y, z, w; };
	};
};

inline Quaternion operator*(f32 s, const Quaternion& q)
{
	return q * s;
}

inline f32 dot(const Quaternion& a, const Quaternion& b)
{
	return dot(a.vector(), b.vector()) + a.w * b.w;
}

inline Quaternion cross(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

inline Quaternion normalize(const Quaternion& q)
{
	return q * (1.0f / q.length());
}

inline Quaternion conjugate(const Quaternion& q)
{
	Quaternion c(-q.vector(), q.w);
	return c;
}

inline Quaternion inverse(const Quaternion& q)
{
	Quaternion c = conjugate(q) / dot(q, q);
	return c;
}

inline Vector3 operator*(const Quaternion& q, const Vector3& v)
{
	//return q * Quaternion(v, 0) * conjugate(q); // More expensive
	Vector3 t = 2.0f * cross(q.vector(), v);
	return (v + q.w * t + cross(q.vector(), t));
}

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

inline Radian angle(const Quaternion& q)
{
	return Radian(2.0f * std::acos(q.w));
}

inline Vector3 axis(const Quaternion& q)
{
	f32 s2 = 1.0 - q.w * q.w; // 1 - cos(theta/2) * cos(theta/2) = sin(theta/2)*sin(theta/2)

	if (s2 <= 0.0f)
		return Vector3(0, 0, 1);

	f32 invs2 = 1.0 / std::sqrt(s2);

	return q.vector() * invs2;
}

inline Quaternion angleAxis(const Radian& angle, const Vector3& axis)
{
	Quaternion q;
	const Vector3 a = normalize(axis);

	const f32 s = std::sin((f32)(0.5f * angle));

	q.vector() = a * s;
	q.w = std::cos((f32)(0.5f * angle));

	return q;
}

struct EulerAngles
{
	Radian pitch;
	Radian yaw;
	Radian roll;
};

inline Radian roll(const Quaternion& q)
{
	return Radian(std::atan2(2.0f * q[0] * q[1] + q[2] * q[3], q[0] * q[0] + q[3] * q[3] - q[1] * q[1] - q[2] * q[2]));
	//return Radian(std::atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), 1.0f - 2.0f * (q[1] * q[1] - q[2] * q[2])));
}

inline Radian pitch(const Quaternion& q)
{
	return Radian(std::atan2(2.0f * q[1] * q[2] + q[3] * q[0], q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2]));
	//return Radian(std::asin(-2.0f * (q[0] * q[2] - q[3] * q[1])));
}

inline Radian yaw(const Quaternion& q)
{
	return Radian(std::asin(-2.0f * (q[0] * q[2] - q[3] * q[1])));
	//return Radian(std::atan2(2.0f * (q[0] * q[3] + q[1] * q[2]), 1.0f - 2.0f(q[2] * q[2] + q[3] * q[3])));
}

inline EulerAngles quaternionToEulerAngles(const Quaternion& q)
{
	return{ pitch(q), yaw(q), roll(q) };
}

inline Matrix4 quaternionToMatrix4(const Quaternion& q)
{
	Matrix4 mat(1);
	Quaternion n = normalize(q);

	const f32 xx = n.x * n.x;
	const f32 yy = n.y * n.y;
	const f32 zz = n.z * n.z;
	const f32 xy = n.x * n.y;
	const f32 xz = n.x * n.z;
	const f32 yz = n.y * n.z;
	const f32 wx = n.w * n.x;
	const f32 wy = n.w * n.y;
	const f32 wz = n.w * n.z;

	mat[0][0] = 1.0f - 2.0f * (yy + zz);
	mat[0][1] = 2.0f * (xy + wz);
	mat[0][2] = 2.0f * (xz - wy);

	mat[1][0] = 2.0f * (xy - wz);
	mat[1][1] = 1.0f - 2.0f * (xx + zz);
	mat[1][2] = 2.0f * (yz + wx);

	mat[2][0] = 2.0f * (xz + wy);
	mat[2][1] = 2.0f * (yz - wx);
	mat[2][2] = 1.0f - 2.0f * (xx + yy);

	return mat;
}
//TODO(Dave): implement rotate, quat<-Mat4

}

#endif // !DUNJUN_MATH_QUATERNION_HPP
