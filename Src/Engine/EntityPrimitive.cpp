#include "EntityPrimitive.h"
#include "Scene.h"
#include "SceneRS.h"

namespace UEngine
{
	void EntityPrimitive::MarkRSDirty()
	{
		if (mRenderStateDirty == false && mRenderState)
		{
			mRenderStateDirty = true;
			GetScene()->mDirtyRSEntities.Add(this);
		}
	}

	void EntityPrimitive::MarkRSTransformDirty()
	{
		if (mRenderStateDirty) return;

		if (mRenderStateTransformDirty == false && mRenderState)
		{
			GetScene()->mDirtyTransformEntities.Add(this);
			mRenderStateTransformDirty = true;
		}
	}

	EntityPrimitiveRS::EntityPrimitiveRS(EntityPrimitive* owner) :
		mGS(owner),
		mSceneRS(owner->GetScene()->GetRS()),
		mLocalToWorld(owner->GetWorldTransform()),
		mBound(owner->GetBound()),
		mLayers(owner->mLayers),
		mWireframeColor(owner->mWireframeColor)
	{

	}


	void EntityPrimitiveRS::Initilize()
	{
		mIndex = mSceneRS->mEntities_RenderState.Add(this);
		mSceneRS->mEntities_BaseData.Add();

		auto& baseData = mSceneRS->mEntities_BaseData[mIndex];
		baseData.mBound = mBound;
		baseData.mLayers = mLayers;
		baseData.mFlags = mFlags;
	}

	void EntityPrimitiveRS::DeInitilize()
	{
		unsigned lastIndex = mSceneRS->mEntities_RenderState.Length();
		mSceneRS->mEntities_RenderState[lastIndex]->mIndex = mIndex;
		mSceneRS->mEntities_RenderState.RemoveAtSwap(mIndex);
		mSceneRS->mEntities_BaseData.RemoveAtSwap(mIndex);
	}

	void EntityPrimitiveRS::UpdateTransform(EntityPrimitive* pOwner)
	{
		mLocalToWorld = pOwner->GetWorldTransform();
		mBound = pOwner->GetBound();
		mSceneRS->mEntities_BaseData[mIndex].mBound = mBound;
	}
};