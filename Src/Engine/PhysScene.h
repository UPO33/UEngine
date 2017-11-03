#pragma once

#include "PhysForward.h"


namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	class Scene;

	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API PhysScene
	{
	public:
		PxPhysics*	mPhys;
		PxScene*	mPXScene;
		Scene*		mScene;

		PhysScene(PxPhysics* physSDK, Scene* owner);

	};
};

