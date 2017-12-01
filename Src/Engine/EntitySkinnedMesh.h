#pragma once

#include "Entity.h"
#include "Asset.h"
#include "EntityPrimitive.h"

namespace UEngine
{
	//maximum number of bone influence per vertex
	static const unsigned MAX_BONE_PER_VERTEX = 4;
	//maximum number of bone a skinned mesh could have
	static const unsigned MAX_BONE = 255;

	struct SkinnedMeshVertexData
	{
		byte	mBoneIDS[MAX_BONE_PER_VERTEX];
		float	mWeights[MAX_BONE_PER_VERTEX];
	};
	struct SkinnedMeshBone
	{
		Name						mNameFull;	//e.g "LefClavicle.UpperArm.ForeArm.Hand
		Name						mName;		//e.g "Hand"
		unsigned					mIndex;
		Transform					mTransform;
		SkinnedMeshBone*			mParent;
		TArray<SkinnedMeshBone*>	mChildren;
	};

	//////////////////////////////////////////////////////////////////////////
	class AAnimSequence;
	class EntitySkinnedMesh;


	class AnimKeyFrame
	{
		float	mTime;
		Vec3	mLocation;
		Vec3	mScale;
		Quat	mRotation;
	};

	//////////////////////////////////////////////////////////////////////////
	class AnimEventBase : public Object
	{
		UCLASS(AnimEventBase, Object)

		friend AAnimSequence;
		friend AnimKeyFrame;

		virtual void OnFire(EntitySkinnedMesh* ownerEntity, const AAnimSequence* ownerAnimation) {}
		float GetFireTime() const { return mFireTime; }

		AAnimSequence* GetOwner() const
		{
			//parent of this object must be always an AnimSequence
			return (AAnimSequence*)this->GetObjectParent();
		}
	private:
		float				mFireTime;
	};
	
	//////////////////////////////////////////////////////////////////////////
	class AAnimSequence : public Asset
	{
		UCLASS(AAnimSequence, Asset)

		TArray<AnimKeyFrame>		mFrames;
		//array of animation events sorted by less mFireTime 
		TArray<AnimEventBase*>		mEvents;

		AnimEventBase* AddEvent(TSubClass<AnimEventBase> eventClass, float fireTime)
		{
			AnimEventBase* newEvent = NewObject<AnimEventBase>(eventClass, this);
			newEvent->mFireTime = fireTime;
			mEvents.Add(newEvent);
		}
		virtual void OnPostLoad()
		{
			ForEachChild(false, false, [this](Object* child){
				if (AnimEventBase* animEvent = child->Cast<AnimEventBase>())
				{
					this->mEvents.AddUnique(animEvent);
				 }
			});
		}
	};

	struct SkinnedMeshInstanceBones
	{
		Transform*		mBonesTransform;
		unsigned*		mBonesParentIndex;
		unsigned		mNumBone;

		unsigned GetRootIndex() const { return 0; }
	};

	class ASkinnedMesh : public Asset
	{
		UCLASS(ASkinnedMesh, Asset)
	};





	class EntitySkinnedMeshBoneForward : public Entity
	{
		UCLASS(EntitySkinnedMeshBoneForward, Entity)

		virtual const Transform& GetWorldTransform() const;
		virtual const Transform& GetLocalTransform() const;

	private:
		Name		mBoneName;

	};

	class EntitySkinnedMesh : public EntityPrimitive
	{
		UCLASS(EntitySkinnedMesh, EntityPrimitive)

	private:
		ASkinnedMesh*			mMesh;
	};


};