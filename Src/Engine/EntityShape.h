#pragma once

#include "EntityPrimitive.h"
#include "PhysForward.h"

namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API EntityShape : public EntityPrimitive
	{
		UCLASS(EntityShape, EntityPrimitive)

		bool					mIsTrigger = false;
		physx::PxRigidBody*		mPhysActor = nullptr;


		virtual void AddForce(const Vec3& force) {}
		virtual void AddImpuse(const Vec3& impluse) {}
		
		virtual Vec3 GetLinearVelocity() { return Vec3(0.0f); }
		virtual Vec3 GetAngularVelocity() { return Vec3(0.0f); }

		
	};

	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API EntityBox : public EntityShape
	{
		UCLASS(EntityBox, EntityShape)

		Vec3 mExtent;
	};

	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API EntitySphere : public EntityShape
	{
		UCLASS(EntitySphere, EntityShape)


		float mRadius = 100;
	};

};
