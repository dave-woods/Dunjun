#include <Dunjun/Camera.hpp>

#include <Dunjun/Math.hpp>

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
		//transform.orientation = conjugate(quaternionLookAt(transform.position, position, up));
		transform.orientation = conjugate(matrix4ToQuaternion(matrix4LookAt(transform.position, position, up)));
	}
	void Camera::offsetOrientation(const Radian& yaw, const Radian& pitch)
	{
		Quaternion yawRot = angleAxis(yaw, { 0, 1, 0 }); //absolute up
		Quaternion pitchRot = angleAxis(pitch, right()); // relative right

		transform.orientation = yawRot * pitchRot * transform.orientation;
	}

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
		else if (projectionType == ProjectionType::InfinitePerspective)
		{
			m = infinitePerspective(fieldOfView, viewportAspectRatio, nearPlane);
		}
		else if (projectionType == ProjectionType::Orthographic)
		{
			f32 distance = 0.5 * (farPlane - nearPlane);
			m = ortho(orthoScale * viewportAspectRatio, -orthoScale * viewportAspectRatio,
				orthoScale, -orthoScale, -distance, distance);
		}

		return m;
	}
	Matrix4 Camera::getView() const
	{
		Matrix4 m;
		
		m = scale(Vector3(1) / transform.scale) * quaternionToMatrix4(conjugate(transform.orientation)) * translate(-transform.position);

		return m;	
	}
}