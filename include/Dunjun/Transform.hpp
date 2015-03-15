#ifndef DUNJUN_TRANSFORM_HPP
#define DUNJUN_TRANSFORM_HPP

#include <Dunjun/Math.hpp>

namespace Dunjun
{
	struct Transform
	{
		Vector3 position;
		Quaternion orientation;
		Vector3 scale;

		Transform()
			: position({ 0, 0, 0 })
			, orientation(Quaternion(0, 0, 0, 1))
			, scale({ 1, 1, 1 })
		{

		}

		//World = Parent * Local
		Transform operator*(const Transform& localSpace) const
		{
			const Transform& parentSpace = *this;
			Transform worldSpace;

			worldSpace.position = parentSpace.position + parentSpace.orientation * (parentSpace.scale * localSpace.position);
			worldSpace.orientation = parentSpace.orientation * localSpace.orientation;
			worldSpace.scale = parentSpace.scale * (parentSpace.orientation * localSpace.scale);

			return worldSpace;
		}

		//Local = World / Parent
		Transform operator/(const Transform& parentSpace) const
		{
			const Transform& worldSpace = *this;
			using namespace Dunjun;
			Transform localSpace;
			const Quaternion psConjugate = conjugate(parentSpace.orientation);

			localSpace.position = (psConjugate * (worldSpace.position - parentSpace.position)) / parentSpace.scale;
			localSpace.orientation = parentSpace.orientation - worldSpace.orientation;
			localSpace.scale = psConjugate * (worldSpace.scale / parentSpace.scale);

			return localSpace;
		}
	};

	inline Transform inverse(const Transform& t)
	{
		const Transform i;
		return i / t;
	}
} //namespace Dunjun

#endif // !DUNJUN_TRANSFORM_HPP
