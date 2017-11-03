#pragma once
#ifdef UENGINE_HAS_PHYSX

#include "Asset.h"
#include "PhysForward.h"


namespace UEngine
{
	enum class EPhysCombineMode
	{
		EAverage = 0,
		EMin,
		EMultiply,
		EMax,
	};

	class UENGINE_API APhysMaterial : public Asset
	{
		UCLASS(APhysMaterial, Asset)

		//Sets the coefficient of dynamic friction.
		//If set to greater than staticFriction, the effective value of staticFriction will be increased to match.
		void				SetDynamicFriction(float coef);
		float				GetDynamicFriction() const { return mDynamicFriction; }
		void				SetStaticFriction(float coef);
		float				GetStaticFriction() const { return mStaticFriction; }
		//Sets the coefficient of restitution
		//A coefficient of 0 makes the object bounce as little as possible, higher values up to 1.0 result in more bounce.
		void				SetRestitution(float rest);
		float				GetRestitution() const { return mRestitution; }
		void				SetFrictionCombineMode(EPhysCombineMode mode);
		EPhysCombineMode	GetFrictionCombineMode() const { return mFrictionCombineMode; }
		void				SetRestitutionCombineMode(EPhysCombineMode mode);
		EPhysCombineMode	GetRestitutionCombineMode() { return mRestitutionCombineMode; }


	private:
		virtual void OnPostPropertyChange(const PropertyChangedEvent&) override;
		void CreatePXMaterial();


		float				mDynamicFriction = 0;
		float				mStaticFriction = 0;
		float				mRestitution = 0;
		EPhysCombineMode	mFrictionCombineMode = EPhysCombineMode::EAverage;
		EPhysCombineMode	mRestitutionCombineMode = EPhysCombineMode::EAverage;

		physx::PxMaterial*	mPXMaterial = nullptr;
	};
};
#endif