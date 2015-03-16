#include <Dunjun/Camera.hpp>

namespace Dunjun
{

	Camera::Camera()
		: transform()
		, fieldOfView(Degree(50))
		, orthoScale(1.0f)
		, nearPlane(0.1f)
		, farPlane(256.0f)
		, viewportAspectRatio(4.0f / 3.0f)
		, projectionType(ProjectionType::Perspective)
	{}

	void Camera::lookAt(const Vector3& position, const Vector3& up)
	{
		const Vector3& pos = transform.position;

		if (length(pos - position) < 0.001f)
			return; // ignore as you cannot you look at where you are

		if (fabs(dot(pos, up) - 1.0f) < 0.001f)
		{
			std::cerr << "Camera::lookat - viewDirection and Up vector are colinear.\n";
			return;
		}

		Matrix4 mat = Dunjun::lookAt(pos, position, up);
		
		transform.orientation = matrix4ToQuaternion(mat);
	}
	void Camera::offsetOrientation(const Radian& yaw, const Radian& pitch)
	{}

	Vector3 Camera::forward() const
	{
		return transform.orientation * Vector3(0, 0, -1);
	}
	Vector3 Camera::backward() const
	{
		return transform.orientation * Vector3(0, 0, +1);
	}

	Vector3 Camera::left() const
	{
		return transform.orientation * Vector3(-1, 0, 0);
	}
	Vector3 Camera::right() const
	{
		return transform.orientation * Vector3(+1, 0, 0);
	}

	Vector3 Camera::down() const
	{
		return transform.orientation * Vector3(0, -1, 0);
	}
	Vector3 Camera::up() const
	{
		return transform.orientation * Vector3(0, +1, 0);
	}

	Matrix4 Camera::getMatrix() const
	{
		return getProjection() * getView();
	}
	Matrix4 Camera::getProjection() const
	{
		Matrix4 m;
		
		if (projectionType == ProjectionType::Perspective)
		{
			m = perspective(fieldOfView, viewportAspectRatio, nearPlane, farPlane);
		}
		else if (projectionType == ProjectionType::Orthographic)
		{
			f32 distance = 0.5 * (farPlane - nearPlane);
			m = ortho(-orthoScale * viewportAspectRatio, orthoScale * viewportAspectRatio,
				-orthoScale, orthoScale, -distance, distance);
		}

		return m;
	}
	Matrix4 Camera::getView() const
	{
		Matrix4 m;
		
		m = scale(Vector3(1) / transform.scale) * quaternionToMatrix4(transform.orientation) * translate(-transform.position);

		return m;	
	}
}