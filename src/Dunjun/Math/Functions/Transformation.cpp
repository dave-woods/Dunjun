#include <Dunjun/Math/Functions/Transformation.hpp>

#include <cassert>
#include <Dunjun/Math/Functions.hpp>

namespace Dunjun
{
	namespace Math
	{
		Matrix4 translate(const Vector3& v)
		{
			Matrix4 result;
			result[3] = Vector4(v, 1);
			return result;
		}

		//Angle in radians
		Matrix4 rotate(const Radian& angle, const Vector3& v)
		{
			const f32 c = Math::cos(angle);
			const f32 s = Math::sin(angle);

			const Vector3 axis(normalize(v));
			const Vector3 t = (1.0f - c) * axis;

			Matrix4 rot;
			rot[0][0] = c + t[0] * axis[0];
			rot[0][1] = 0 + t[0] * axis[1] + s * axis[2];
			rot[0][2] = 0 + t[0] * axis[2] - s * axis[1];
			rot[0][3] = 0;

			rot[1][0] = 0 + t[1] * axis[0] - s * axis[2];
			rot[1][1] = c + t[1] * axis[1];
			rot[1][2] = 0 + t[1] * axis[2] + s * axis[0];
			rot[1][3] = 0;

			rot[2][0] = 0 + t[2] * axis[0] + s * axis[1];
			rot[2][1] = 0 + t[2] * axis[1] - s * axis[0];
			rot[2][2] = c + t[2] * axis[2];
			rot[2][3] = 0;

			return rot;
		}

		Matrix4 scale(const Vector3& v)
		{
			Matrix4 result({ v.x, 0, 0, 0 }, { 0, v.y, 0, 0 }, { 0, 0, v.z, 0 }, { 0, 0, 0, 1.0f });


			return result;
		}

		Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top)
		{
			Matrix4 result;

			result[0][0] = 2.0f / (right - left);
			result[1][1] = 2.0f / (top - bottom);
			result[2][2] = -1.0f;
			result[3][0] = -(right + left) / (right - left);
			result[3][1] = -(top + bottom) / (top - bottom);

			return result;
		}

		Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
		{
			Matrix4 result;

			result[0][0] = 2.0f / (right - left);
			result[1][1] = 2.0f / (top - bottom);
			result[2][2] = -2.0f / (zFar - zNear);
			result[3][0] = -(right + left) / (right - left);
			result[3][1] = -(top + bottom) / (top - bottom);
			result[3][2] = -(zFar + zNear) / (zFar - zNear);

			return result;
		}

		Matrix4 perspective(const Radian& fovy, f32 aspect, f32 zNear, f32 zFar)
		{
			assert(Math::abs(aspect - std::numeric_limits<f32>::epsilon()) > 0.0f);

			const f32 tanHalfFovy = Math::tan(0.5 * fovy);

			Matrix4 result(0.0f);

			result[0][0] = 1.0f / (aspect * tanHalfFovy);
			result[1][1] = 1.0f / (tanHalfFovy);
			result[2][2] = -(zFar + zNear) / (zFar - zNear);
			result[2][3] = -1.0f;
			result[3][2] = -2.0f * zFar * zNear / (zFar - zNear);

			return result;
		}

		Matrix4 infinitePerspective(const Radian& fovy, f32 aspect, f32 zNear)
		{
			const f32 range = Math::tan(0.5 * fovy) * zNear;
			const f32 left = -range * aspect;
			const f32 right = range * aspect;
			const f32 bottom = -range;
			const f32 top = range;

			Matrix4 result(0.0f);

			result[0][0] = (2.0f * zNear) / (right - left);
			result[1][1] = (2.0f * zNear) / (top - bottom);
			result[2][2] = -1.0f;
			result[2][3] = -1.0f;
			result[3][2] = -2.0f * zNear;

			return result;
		}


		template<>
		Matrix4 lookAt<Matrix4>(const Vector3& eye, const Vector3& center, const Vector3& up)
		{
			const Vector3 f(normalize(center - eye));
			const Vector3 s(normalize(cross(f, up)));
			const Vector3 u(cross(s, f));

			Matrix4 result;

			result[0][0] = +s.x;
			result[1][0] = +s.y;
			result[2][0] = +s.z;

			result[0][1] = +u.x;
			result[1][1] = +u.y;
			result[2][1] = +u.z;

			result[0][2] = -f.x;
			result[1][2] = -f.y;
			result[2][2] = -f.z;

			result[3][0] = -dot(s, eye);
			result[3][1] = -dot(u, eye);
			result[3][2] = +dot(f, eye);

			return result;
		}

		template<>
		Quaternion lookAt<Quaternion>(const Vector3& eye, const Vector3& center, const Vector3& up)
		{
			const f32 similar = 0.001f;

			if (length(center - eye) < similar)
				return Quaternion(); // ignore as you cannot you look at where you are

			return matrix4ToQuaternion(lookAt<Matrix4>(eye, center, up));
			/*const Vector3 f(normalize(center - eye));
			const Vector3 s(normalize(cross(f, up)));
			const Vector3 u(cross(s, f));
			const Vector3 refUp(normalize(up));

			const f32 m = std::sqrt(2.0f + 2.00 * dot(u, refUp));
			Vector3 v = (1.0f / m) * cross(u, refUp);
			return Quaternion(v, 0.5f * m);*/
		}
	}
}
