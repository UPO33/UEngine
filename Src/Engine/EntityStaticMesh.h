#pragma once

#include "Entity.h"
#include "StaticMesh.h"


namespace UEngine
{
#ifdef TEST
	//////////////////////////////////////////////////////////////////////////
	class AStaticMesh;

	using StaticMeshMaterialArrayT = TArray<TObjectPtr<AMaterial>>;


	class UENGINE_API EntityStaticMesh : public EntityPrimitive
	{
		UCLASS(EntityStaticMesh, EntityPrimitive)

		virtual void SetMesh(AStaticMesh* newMesh);
		AStaticMesh* GetMesh() const { return mMesh.Get(); }


		virtual void SetMaterial(unsigned index, AMaterial* material) override;
		virtual void SetMaterial(Name name, AMaterial* material) override;
		virtual AMaterial* GetMaterial(unsigned index) override;
		virtual AMaterial* GetMaterial(Name tag) override;
		virtual unsigned NumMaterial() const override;

		virtual EntityPrimitiveRS* CreateRS() override
		{
			if(mMesh && mMesh->IsRenderValid())
				return new EntityStaticMeshRS(this);

			return nullptr;
		}
		virtual AABB CalculateBound(const Transform& worldTransfom) const override
		{
			if (mMesh)
			{
				UASSERT(mBoundScale >= 0);
				return AABB::TransformAffine(mMesh->GetBoundingBox(), worldTransfom) * mBoundScale;
			}
			return ParentT::CalculateBound(worldTransfom);
		}
	private:
		TObjectPtr<AStaticMesh>				mMesh;
		bool								mUseOverrideMaterials;
		StaticMeshMaterialArrayT			mOverrideMaterials;
		
	};

	enum EEntityRSFlag
	{
		ERS_HiddenInGame,
		ERS_HiddenInEditor,
		ERS_IsEditor,
	};



	class PrimitiveCollector
	{
	};
	class StaticMeshCollector
	{
	};
	class PrimitiveBatch;

	static const Color32 BOUND_COLOR_NORMAL;
	static const Color32 BOUND_COLOR_SELECTED;



	class UENGINE_API EntityStaticMeshRS : public EntityPrimitiveRS
	{
		EntityStaticMeshRS(EntityStaticMesh* pOwner) : EntityPrimitiveRS(pOwner)
			mMesh(pOwner->mMesh->GetRS()),
		{
			 
		}
		AStaticMeshRS*		mMesh;

		unsigned		mNumMaterial;
		unsigned		mMaxLODIndex;
		unsigned		mMinLODIndex;

		struct LODData
		{
			GFXPiplineState* mPSODepthOnly;
			GFXPiplineState* mPSOMainPass;
		};
		

		EntityStaticMesh* GetGS() const { return (EntityStaticMesh*)mGS; }

		void DynamicPass_DepthOnly(SceneView& view, HGFXCmdList cmdList)
		{
			cmdList.Set
				cmdList.SetPipline();
		}
		struct DynamicPassCollector
		{
		};
		void DynamicPass(const SceneView& view, DynamicPassCollector& collector)
		{
			unsigned LODIndex = CalcLOD(view);
			
			collector.AddMesh(mMesh->mLODS[LODIndex]);
		}
		//////////////////////////////////////////////////////////////////////////
		unsigned CalcLOD(const SceneView& view) const
		{
			if (view.mViewOptions.mForceLODIndex != -1)
			{
				return Max(view.mViewOptions.mForceLODIndex, mMesh->mMaxLODIndex);
			}


			float LODAplha = (view.GetPosition() - GetWorldPosition()).LengthSquare() * view.mLODCoefficient; 
			for (unsigned iLOD = mMesh->mMaxLODIndex; iLOD != 0; iLOD--)
			{
				if (LODAplha >= mMesh->mLODS[iLOD].mSizeBasedLODValue)
					return iLOD;
			}
			return 0;
		}
		AABB GetOcclusionBound() const
		{

		}
		
	};

#endif

};

