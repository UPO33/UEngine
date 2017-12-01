#include "DebugAsset.h"

namespace UEngine
{
	UCLASS_BEGIN_IMPL(ADebugAsset, AttrIcon("Note.png"))
		UPROPERTY(mFloat0)
		UPROPERTY(mInt0)
		UPROPERTY(mBool0)
		UPROPERTY(mName)
		UPROPERTY(mVec2)
		UPROPERTY(mVec3)
		UPROPERTY(mVec4)
		UPROPERTY(mColor)
		UPROPERTY(mColor32)
		UPROPERTY(mAssetPtr0)
		UPROPERTY(mArrayOfVec3)
		UPROPERTY(mArrayOfName)
		UPROPERTY(mSubClass)
		UPROPERTY(mChildObject, )
	UCLASS_END_IMPL(ADebugAsset)
};