#ifdef UENGINE_HAS_PHYSX

#include "PhysMaterial.h"
#include "PhysAPI.h"

namespace UEngine
{
	UCLASS_BEGIN_IMPL(APhysMaterial)
		UPROPERTY(mDynamicFriction)
		UPROPERTY(mStaticFriction)
		UPROPERTY(mRestitutionCombineMode)
		UPROPERTY(mFrictionCombineMode)
	UCLASS_END_IMPL(APhysMaterial)
};

namespace UEngine
{
	PxCombineMode::Enum ToNVPX(EPhysCombineMode in)
	{
		return (PxCombineMode::Enum)in;
	}

	APhysMaterial::~APhysMaterial()
	{
		if (mPXMaterial) mPXMaterial->release();
		mPXMaterial = nullptr;
	}

	void APhysMaterial::SetDynamicFriction(float coef)
	{
		mDynamicFriction = coef;
		mPXMaterial->setDynamicFriction(mDynamicFriction);
	}

	void APhysMaterial::SetStaticFriction(float coef)
	{
		mStaticFriction = coef;
		mPXMaterial->setStaticFriction(mStaticFriction);
	}

	void APhysMaterial::SetRestitution(float rest)
	{
		mRestitution = rest;
		mPXMaterial->setRestitution(mRestitution);
	}

	void APhysMaterial::SetFrictionCombineMode(EPhysCombineMode mode)
	{
		mFrictionCombineMode = mode;
		mPXMaterial->setFrictionCombineMode(ToNVPX(mFrictionCombineMode));
	}

	void APhysMaterial::SetRestitutionCombineMode(EPhysCombineMode mode)
	{
		mRestitutionCombineMode = mode;
		mPXMaterial->setRestitutionCombineMode(ToNVPX(mRestitutionCombineMode));
	}

	void APhysMaterial::OnAfterPropertyChange(const PropertyChangedEvent& pce)
	{
		mPXMaterial->setDynamicFriction(mDynamicFriction);
		mPXMaterial->setStaticFriction(mStaticFriction);
		mPXMaterial->setRestitution(mRestitution);
		mPXMaterial->setRestitutionCombineMode(ToNVPX(mRestitutionCombineMode));
		mPXMaterial->setFrictionCombineMode(ToNVPX(mFrictionCombineMode));

		ParentT::OnAfterPropertyChange(pce);
	}

	void APhysMaterial::OnPostConstruction()
	{
		CreatePXMaterial();
	}
	void APhysMaterial::OnPostClone()
	{
		mPXMaterial = nullptr;

		ParentT::OnPostClone();
	}

	void APhysMaterial::CreatePXMaterial()
	{
		UASSERT(mPXMaterial == nullptr);
		mPXMaterial = UGetNVPhysicInstance()->createMaterial(mStaticFriction, mDynamicFriction, mRestitution);
		UASSERT(mPXMaterial);
		mPXMaterial->userData = this;
		mPXMaterial->setRestitutionCombineMode(ToNVPX(mRestitutionCombineMode));
		mPXMaterial->setFrictionCombineMode(ToNVPX(mFrictionCombineMode));
	}

};

#endif