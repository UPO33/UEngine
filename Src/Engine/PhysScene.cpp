#include "PhysScene.h"
#include "PhysAPI.h"
#include "../Core/Vector.h"

namespace UEngine
{
	inline const PxVec3& AsPX(const Vec3& v)
	{
		return *((PxVec3*)&v);
	}


	PhysScene::PhysScene(PxPhysics* phys, Scene* owner)
	{
		
	}



};