#ifndef DUNJUN_MATH_FUNCTIONS_INTERPOLATION_HPP
#define DUNJUN_MATH_FUNCTIONS_INTERPOLATION_HPP

#include <Dunjun/Common.hpp>
#include <Dunjun/Math/Quaternion.hpp>

#include <Dunjun/Math/Functions/Common.hpp>

namespace Dunjun
{
	namespace Math
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
				Radian angle = Math::acos(cosTheta);

				result = Math::sin(Radian(1.0f) - (t * angle)) * a + Math::sin(t * angle) * q;
				result = result * (1.0f / Math::sin(angle));
			}
			return result;
		}

		// Shoemake's Quaternion Curves
		// Spherical Cubic Interpolation
		inline Quaternion squad(const Quaternion& p, const Quaternion& a, const Quaternion& b, const Quaternion& q, f32 t)
		{
			return slerp(slerp(p, q, t), slerp(a, b, t), 2.0f * t * (1.0f - t));
		}
	}
}

#endif // !DUNJUN_MATH_FUNCTIONS_INTERPOLATION_HPP
