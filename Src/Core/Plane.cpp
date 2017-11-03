#include "Plane.h"



namespace UCore
{
	Plane::Plane(const Vec3& point0, const Vec3& point1, const Vec3& point2)
	{
		Vec3 n = ~(point1 - point0) ^ (point2 - point0);
		x = n.x;
		y = n.y;
		z = n.z;
		d = -(n | point0);
	}
}