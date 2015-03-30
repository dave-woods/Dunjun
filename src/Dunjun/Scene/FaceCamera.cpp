#include <Dunjun/Scene/FaceCamera.hpp>
#include <Dunjun/Scene/SceneNode.hpp>
#include <Dunjun/Math.hpp>

namespace Dunjun
{
	FaceCamera::FaceCamera(const Camera& camera, const Vector3& up)
		: camera(&camera)
		, up(up)
	{}

	void FaceCamera::update(f32 dt)
	{
		Vector3 pos = parent->getGlobalTransform().position;
		Vector3 camPos = camera->transform.position;

		Matrix4 orientation = matrix4LookAt(camPos, pos, up);

		parent->transform.orientation = conjugate(matrix4ToQuaternion(orientation));
	}
}