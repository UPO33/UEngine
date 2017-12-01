#include "EntitySkinnedMesh.h"

namespace UEngine
{
	UCLASS_BEGIN_IMPL(AnimEventBase)
		UPROPERTY(mFireTime)
	UCLASS_END_IMPL(AnimEventBase)

	UCLASS_BEGIN_IMPL(AAnimSequence)
	UCLASS_END_IMPL(AAnimSequence)
};

namespace UEngine
{

	const Transform& EntitySkinnedMeshBoneForward::GetWorldTransform() const
	{
		//#TODO
		return Transform::IDENTITY;
	}

	const Transform& EntitySkinnedMeshBoneForward::GetLocalTransform() const
	{
		//#TODO
		return Transform::IDENTITY;
	}

};
