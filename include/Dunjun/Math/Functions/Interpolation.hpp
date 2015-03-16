#ifndef DUNJUN_MATH_FUNCTIONS_INTERPOLATION_HPP
#define DUNJUN_MATH_FUNCTIONS_INTERPOLATION_HPP

#include <Dunjun/Common.hpp>
#include <Dunjun/Math/Quaternion.hpp>

#include <cmath>

namespace Dunjun
{
	template <class T>
	inline T lerp(const T& x, const T& y, f32 t)
	{
		assert(t >= 0.0f && t <= 1.0f);

		return x * (1.0f - t) + (y * t);
	}

	inline Quaternion slerp(const Quaternion& a, const Quaternion& b, f32 t)
	{
		assert(t >= 0.0f && t <= 1.0f);

		Quaternion result;
		Quaternion q = b;
		f32 cosTheta = dot(a, b);

		if (cosTheta < 0.0f)
		{
			q = -b;
			cosTheta = -cosTheta;
		}
		if (cosTheta > 1.0f)
		{
			result = Quaternion(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t));
		}
		else
		{
			f32 angle = std::acos(cosTheta);

			result = std::sin(1.0f - t * angle) * a + std::sin(t * angle) * q;
			result = result * (1.0f / std::sin(angle));
		}
		return result;
	}
}

#endif // !DUNJUN_MATH_FUNCTIONS_INTERPOLATION_HPP
