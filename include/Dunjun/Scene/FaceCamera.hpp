#ifndef DUNJUN_SCENE_FACECAMERA_HPP
#define DUNJUN_SCENE_FACECAMERA_HPP

#include <Dunjun/Scene/NodeComponent.hpp>
#include <Dunjun/Camera.hpp>

namespace Dunjun
{
	class FaceCamera : public NodeComponent
	{
	public:
		explicit FaceCamera(const Camera& camera, const Vector3& up = { 0, 1, 0 });

		virtual void update(f32 dt) override;

		const Camera* camera;
		Vector3 up;
	};
}

#endif // !DUNJUN_SCENE_FACECAMERA_HPP
