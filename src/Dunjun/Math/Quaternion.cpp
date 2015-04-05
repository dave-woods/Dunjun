#include <Dunjun/Math/Quaternion.hpp>
#include <Dunjun/Math/Functions.hpp>

namespace Dunjun
{

	Quaternion::Quaternion()
		: x(0)
		, y(0)
		, z(0)
		, w(1)
	{
	}

	Quaternion::Quaternion(f32 x, f32 y, f32 z, f32 w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{

	}

	Quaternion::Quaternion(const Vector3& v, f32 s)
		: x(v.x)
		, y(v.y)
		, z(v.z)
		, w(s)
	{
	}

	Quaternion Quaternion::operator-() const
	{
		Quaternion c;
		c.x = -x;
		c.y = -y;
		c.z = -z;
		c.w = -w;
		return c;
	}

	Quaternion Quaternion::operator+(const Quaternion& b) const
	{
		Quaternion c;
		c.x = x + b.x;
		c.y = y + b.y;
		c.z = z + b.z;
		c.w = w + b.w;
		return c;
	}

	Quaternion Quaternion::operator-(const Quaternion& b) const
	{
		Quaternion c;
		c.x = x - b.x;
		c.y = y - b.y;
		c.z = z - b.z;
		c.w = w - b.w;
		return c;
	}

	Quaternion Quaternion::operator*(const Quaternion& b) const
	{
		const Quaternion& a = *this;
		Quaternion c;

		// q = (v, s) -> where v = Vector3, s = Scalar (f32)
		// (v1, s1)(v2, s2) = (s1v2 + s2v1 + v1 x v2, s1s2 - v1 . v2)
		// x => cross product
		// . => dot product

		c.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
		c.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
		c.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
		c.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
		return c;
	}

	Quaternion Quaternion::operator*(f32 s) const
	{
		Quaternion c = *this;

		c.x *= s;
		c.y *= s;
		c.z *= s;
		c.w *= s;
		return c;
	}

	Quaternion Quaternion::operator/(f32 s) const
	{
		Quaternion c = *this;

		c.x /= s;
		c.y /= s;
		c.z /= s;
		c.w /= s;
		return c;
	}

	bool Quaternion::operator==(const Quaternion& b) const
	{
		for (usize i = 0; i < 0; i++)
			if (data[i] != b.data[i])
				return false;
		return true;
	}

	bool Quaternion::operator!=(const Quaternion& b) const
	{
		return !operator==(b);
	}

	const Vector3 Quaternion::vector() const { return (const Vector3&)(data); }
	Vector3& Quaternion::vector() { return (Vector3&)(data); }

	f32 Quaternion::scalar() const { return w; }
	f32& Quaternion::scalar() { return w; }

	f32 lengthSquared(const Quaternion& q)
	{
		return dot(q, q);
	}

	f32 length(const Quaternion& q)
	{
		return Math::sqrt(lengthSquared(q));
	}

	f32 dot(const Quaternion& a, const Quaternion& b)
	{
		return dot(a.vector(), b.vector()) + a.w * b.w;
	}

	Quaternion cross(const Quaternion& a, const Quaternion& b)
	{
		return Quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
			a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
			a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
			a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
	}

	Quaternion normalize(const Quaternion& q)
	{
		return q * (1.0f / length(q));
	}

	Quaternion conjugate(const Quaternion& q)
	{
		Quaternion c(-q.vector(), q.w);
		return c;
	}

	Quaternion inverse(const Quaternion& q)
	{
		Quaternion c = conjugate(q) / dot(q, q);
		return c;
	}

	Vector3 operator*(const Quaternion& q, const Vector3& v)
	{
		//return q * Quaternion(v, 0) * conjugate(q); // More expensive
		Vector3 t = 2.0f * cross(q.vector(), v);
		return (v + q.w * t + cross(q.vector(), t));
	}

	Radian angle(const Quaternion& q)
	{
		return 2.0f * Math::acos(q.w);
	}

	Vector3 axis(const Quaternion& q)
	{
		f32 s2 = 1.0 - q.w * q.w; // 1 - cos(theta/2) * cos(theta/2) = sin(theta/2)*sin(theta/2)

		if (s2 <= 0.0f)
			return Vector3(0, 0, 1);

		f32 invs2 = 1.0 / Math::sqrt(s2);

		return q.vector() * invs2;
	}

	Quaternion angleAxis(const Radian& angle, const Vector3& axis)
	{
		Quaternion q;
		const Vector3 a = normalize(axis);

		const f32 s = Math::sin(0.5f * angle);

		q.vector() = a * s;
		q.w = Math::cos(0.5f * angle);

		return q;
	}

	Radian roll(const Quaternion& q)
	{
		return Math::atan2(2.0f * q[0] * q[1] + q[2] * q[3], q[0] * q[0] + q[3] * q[3] - q[1] * q[1] - q[2] * q[2]);
	}

	Radian pitch(const Quaternion& q)
	{
		return Math::atan2(2.0f * q[1] * q[2] + q[3] * q[0], q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2]);
	}

	Radian yaw(const Quaternion& q)
	{
		return Math::asin(-2.0f * (q[0] * q[2] - q[3] * q[1]));
	}

	EulerAngles quaternionToEulerAngles(const Quaternion& q)
	{
		return{ pitch(q), yaw(q), roll(q) };
	}

	Quaternion eulerAnglesToQuaternion(const EulerAngles& e, const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
	{
		Quaternion p = angleAxis(e.pitch, xAxis);
		Quaternion y = angleAxis(e.pitch, yAxis); // should be yaw?
		Quaternion r = angleAxis(e.pitch, zAxis); // should be roll?
	
		return y * p * r;
	}

	Matrix4 quaternionToMatrix4(const Quaternion& q)
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
	
	//Assumes matrix is only rotational without skew
	Quaternion matrix4ToQuaternion(const Matrix4& m)
	{
		f32 fourXSquaredMinusOne = m[0][0] - m[1][1] - m[2][2];
		f32 fourYSquaredMinusOne = m[1][1] - m[0][0] - m[2][2];
		f32 fourZSquaredMinusOne = m[2][2] - m[0][0] - m[1][1];
		f32 fourWSquaredMinusOne = m[2][2] + m[0][0] + m[1][1];

		int biggestIndex = 0;
		f32 fourBiggestSquaredMinusOne = fourWSquaredMinusOne;
		if (fourXSquaredMinusOne > fourBiggestSquaredMinusOne)
		{
			fourBiggestSquaredMinusOne = fourXSquaredMinusOne;
			biggestIndex = 1;
		}
		if (fourYSquaredMinusOne > fourBiggestSquaredMinusOne)
		{
			fourBiggestSquaredMinusOne = fourYSquaredMinusOne;
			biggestIndex = 2;
		}
		if (fourZSquaredMinusOne > fourBiggestSquaredMinusOne)
		{
			fourBiggestSquaredMinusOne = fourZSquaredMinusOne;
			biggestIndex = 3;
		}

		f32 biggestValue = Math::sqrt(fourBiggestSquaredMinusOne + 1.0f) * 0.5f;
		f32 mult = 0.25f / biggestValue;

		Quaternion q;

		switch (biggestIndex)
		{
		case 0:
			q.w = biggestValue;
			q.x = (m[1][2] - m[2][1]) * mult;
			q.y = (m[2][0] - m[0][2]) * mult;
			q.z = (m[0][1] - m[1][0]) * mult;
			break;
		case 1:
			q.w = (m[1][2] - m[2][1]) * mult;
			q.x = biggestValue;
			q.y = (m[0][1] + m[1][0]) * mult;
			q.z = (m[2][0] + m[0][2]) * mult;
			break;
		case 2:
			q.w = (m[2][0] - m[0][2]) * mult;
			q.x = (m[0][1] + m[1][0]) * mult;
			q.y = biggestValue;
			q.z = (m[1][2] + m[2][1]) * mult;
			break;
		case 3:
			q.w = (m[0][1] - m[1][0]) * mult;
			q.x = (m[2][0] + m[0][2]) * mult;
			q.y = (m[1][2] + m[2][1]) * mult;
			q.z = biggestValue;
			break;
		default: //Should never reach this
			assert(false);
			break;
		}

		return q;
	}

}

