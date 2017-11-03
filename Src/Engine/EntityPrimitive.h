#pragma once

#include "EntityNode.h"

namespace UEngine
{


//////////////////////////////////////////////////////////////////////////
class UENGINE_API EntityPrimitiveRS
{
	friend SceneRS;

public:
	EntityPrimitive*	mGS;
	SceneRS*			mSceneRS;
	EntityFlagsT		mFlags;
	EntityLayerT		mLayers;

	Matrix4				mLocalToWorld;
	AABB				mBound;
	size_t				mIndex;
	Color				mWireframeColor;

	size_t				mIsAlive : 1;
	size_t				mIsInvWorldTransformDirty : 1;
	size_t				mIsSelected : 1;
	size_t				mIsHover : 1;
	size_t				mBeginPlayWasCalled : 1;
	size_t				mEndPlayWasCalled : 1;
	size_t				mUseParentBound : 1;
	size_t				mUseParentLOD : 1;
	size_t				mRegistered : 1;
	size_t				mIsPrefab : 1;
	size_t				mIsEditor : 1;
	size_t				mHiddenInGame : 1;
	size_t				mHiddenInEditor : 1;
	size_t				mCastShadow : 1;
	size_t				mRecieveShadow : 1;
	size_t				mDrawMainPass : 1;
	size_t				mDrawCustomDepthPass : 1;

	EntityPrimitiveRS(EntityPrimitive* owner);
	//this is called on render thread
	virtual void Initilize();
	virtual void DeInitilize();

	virtual void Fetch(unsigned flag) {}
	virtual void PostFetch() {}

	virtual void UpdateTransform(EntityPrimitive* pOwner);
};

//////////////////////////////////////////////////////////////////////////
class UENGINE_API EntityPrimitive : public EntityNode
{
	UCLASS(EntityPrimitive, EntityNode)

	EntityPrimitiveRS*	mRenderState = nullptr;
	float				mBoundScale = 1;
	Color				mWireframeColor = Color::GREEN;

	AABB GetBound() { return CalculateBound(GetWorldTransform()); }
	EntityPrimitiveRS* GetRS() const { return mRenderState; }
	//this function is in charge of calculating the world space bounding box of Entity
	virtual AABB					CalculateBound(const Transform& worldTransfom) const
	{
		return AABB(worldTransfom.GetTranslation(), worldTransfom.GetTranslation());
	}
	virtual EntityPrimitiveRS*		CreateRS() { return new EntityPrimitiveRS(this); }

	virtual void					SetMaterial(unsigned index, AMaterial* material) {}
	virtual void					SetMaterial(Name tag, AMaterial* material) {}
	virtual AMaterial*				GetMaterial(unsigned index) { return nullptr; }
	virtual AMaterial*				GetMaterial(Name tag) { return nullptr; }
	virtual unsigned				NumMaterial() const { return 0; }

	virtual void OnRegister()
	{
		ParentT::OnRegister();

		EntityPrimitiveRS* rs = CreateRS();
		mRenderState = rs;

		UEnqueue(EET_Render, [rs]() {
			rs->Initilize();
		});
	}
	virtual void OnDeregister()
	{
		if (EntityPrimitiveRS* rs = mRenderState)
		{
			UEnqueue(EET_Render, [rs]()
			{
				rs->DeInitilize();
			});
		}

		ParentT::OnDeregister();
	}
	void RecreateRS()
	{
		if (mRenderState)
		{
			UEnqueue(EET_Render, [this]() {
				SafeDelete(this->mRenderState);

			});
		}
	}

	bool HasRenderState() const { return mRenderState != nullptr; }

	void MarkRSDirty();
	void MarkRSTransformDirty();






};


};