#pragma once

#include "EntityNode.h"
#include "SceneView.h"

namespace UEngine
{

//////////////////////////////////////////////////////////////////////////
class UENGINE_API EntityCamera : public EntityNode
{
	UCLASS(EntityCamera, EntityNode)

	bool				mIsPerspective = true;
	float				mFiledOfView = 60;
	float				mOrthoSize = 80;
	float				mNearPlane = 0.01;
	float				mFarPlane = 100000;
	unsigned			mRenderLayer = 0xFFffFFff;
	Vec2				mViewportOffset = Vec2(0.0f);
	Vec2				mViewportSize = Vec2(1.0f);


	virtual void OnRegister() override;
	virtual void OnDeregister() override;
};
	

};