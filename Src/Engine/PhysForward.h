#pragma once

#include "Base.h"

namespace physx
{
	class PxMaterial;
	class PxPhysics;
	class PxScene;
	class PxShape;
	class PxRigidBody;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxConstraint;
	class PxSimulationEventCallback;
	class PxBatchQueryDesc;
	class PxBatchQuery;
	class PxAggregate;
	class PxRenderBuffer;
	class PxVolumeCache;
	class PxSphereGeometry;
	class PxBoxGeometry;
	class PxCapsuleGeometry;

	class PxTriangleMesh;
};

namespace UEngine
{
	using namespace physx;

	
	UENGINE_API PxPhysics*	UGetNVPhysicInstance();
};