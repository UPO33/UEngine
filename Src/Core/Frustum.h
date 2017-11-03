#pragma once

#include "Base.h"
#include "Bound.h"
#include "Plane.h"

namespace UCore
{
	enum EFrustumPlane
	{
		EFP_Near, 
		EFP_Far,
		EFP_Right, 
		EFP_Left, 
		EFP_Top, 
		EFP_Bottom
	};

	//////////////////////////////////////////////////////////////////////////
	template< unsigned NumPlanes> struct TConvexVolume
	{
		//the normal of the plane points outward of the volume so that the negative half-space is inside the volume
		Plane mPlanes[NumPlanes];

		bool ContainsPoint(const Vec3& point)
		{
			for (unsigned i = 0; i < NumPlanes; i++)
			{
				if (mPlanes[i].DotCoord(point) > 0)
					return false;
			}
			return true;
		}
	};

	struct UCORE_API Frustum
	{
		static constexpr unsigned NUM_PLANES = 6;

		//The planes are defined such that the normal points towards the inside of the view frustum.		
		Plane	mPlanes[NUM_PLANES];	

		bool ContainsPoint(const Vec3& point) const;
		
		bool Intersects(const AABB& box) const;
	};
};