#include "Frustum.h"

namespace UCore
{

	bool Frustum::ContainsPoint(const Vec3& point) const
	{
		for (unsigned i = 0; i <NUM_PLANES; i++)
		{
			if (mPlanes[i].DotCoord(point) < 0) return false;
		}
		return true;
	}

	bool Frustum::Intersects(const AABB& box) const
	{
		for (unsigned i = 0; i < NUM_PLANES; i++)
		{
			//The positive vertex is the vertex from the box that is further along the normal's direction
			//If the p-vertex is on the wrong side of the plane, the box can be immediately rejected
			Vec3 positiveVertex = box.mMin;

			if (mPlanes[i].x >= 0) positiveVertex.x = box.mMax.x;
			if (mPlanes[i].y >= 0) positiveVertex.y = box.mMax.y;
			if (mPlanes[i].z >= 0) positiveVertex.z = box.mMax.z;

			if (mPlanes[i].DotCoord(positiveVertex) < 0) // is outside ?
				return false;
		}
		return true;
	}

};
