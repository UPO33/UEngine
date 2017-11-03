#pragma once

#include "Vector.h"


namespace UCore
{
	struct UCORE_API alignas(16) Plane
	{
		float x, y, z, d;

		Plane() {}
		Plane(InitZero) { x = y = z = d = 0; }

		Plane(float _x, float _y, float _z, float _d) : x(_x), y(_y), z(_z), d(_d) {}
		Plane(const Vec3& normalizedNormal, float distance)
		{
			x = normalizedNormal.x;
			y = normalizedNormal.y;
			z = normalizedNormal.z;
			d = distance;
		}
		explicit Plane(Vec4& v) : x(v.x), y(v.y), z(v.z), d(v.w) {}
		Plane& operator =(const Vec4& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			d = v.w;
			return *this;
		}
		Plane(const Vec3& point, const Vec3& normal)
		{
			UASSERT(normal.IsNormalized());
			x = normal.x;
			y = normal.y;
			z = normal.z;
			d = -(normal | point);
		}
		Plane(const Vec3& point0, const Vec3& point1, const Vec3& point2);

		void Normalize()
		{
			float lenSQ = x * x + y * y + z * z;
			float rsqrt = RSqrt(lenSQ);
			x *= rsqrt;
			y *= rsqrt;
			z *= rsqrt;
			d *= rsqrt;
		}

		//This function can be useful in finding the signed distance from a point to a plane. 
		float DotCoord(const Vec3& v3) const
		{
			return (x * v3.x) + (y * v3.y) + (z * v3.z) + d;
		}
		//This function is useful for calculating the angle between the normal vector of the plane, and another normal vector.
		float DotNormal(const Vec3& v3) const {
			return (x * v3.x) + (y * v3.y) + (z * v3.z);
		}
		Vec3 GetNormal() const { return Vec3(x, y, z); }
		Vec3 ProjectPoint(const Vec3& point)
		{
			return point - Vec3(x, y, z) * DotCoord(point);
		}

		float& operator [] (unsigned i) { return ((float*)this)[i]; }
		float operator [] (unsigned i) const { return ((float*)this)[i]; }
	};
	inline float Dot(const Plane& a, const Plane& b)
	{
		return Dot((const Vec4&)a, (const Vec4&)b);
	}

	inline float UPlanePointSignedDistance(const Vec3& planeNormal, const Vec3& planePoint, const Vec3& point)
	{
		return planeNormal | (point - planePoint);
	}
}
