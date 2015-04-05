#include <Dunjun/Transform.hpp>

namespace Dunjun
{
	//World = Parent * Local
	Transform operator*(const Transform& ps, const Transform& ls)
	{
		Transform ws;

		ws.position = ps.position + ps.orientation * (ps.scale * ls.position);
		ws.orientation = ps.orientation * ls.orientation;
		ws.scale = ps.scale * (ps.orientation * ls.scale);

		return ws;
	}

	//Local = World / Parent
	Transform operator/(const Transform& ws, const Transform& ps)
	{
		Transform ls;

		const Quaternion psConjugate = conjugate(ps.orientation);

		ls.position = (psConjugate * (ws.position - ps.position)) / ps.scale;
		ls.orientation = psConjugate * ws.orientation;
		ls.scale = psConjugate * (ws.scale / ps.scale);

		return ls;
	}

	Transform& operator*=(Transform& ps, const Transform& ls)
	{
		ps = ps * ls;
		return ps;
	}
	Transform& operator/=(Transform& ws, const Transform& ps)
	{
		ws = ws / ps;
		return ws;
	}


	Transform inverse(const Transform& t)
	{
		const Transform i;
		return i / t;
	}

	Matrix4 transformMatrix4(const Transform& t)
	{
		return Math::translate(t.position) * quaternionToMatrix4(t.orientation) * Math::scale(t.scale);
	}

}