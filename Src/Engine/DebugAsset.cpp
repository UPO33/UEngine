#include "DebugAsset.h"

namespace UEngine
{
	UCLASS_BEGIN_IMPL(ADebugAsset, AttrIcon("Note.png"))
		UPROPERTY(mFloat0)
		UPROPERTY(mFloatMinMax, AttrMinMax(0, 1), AttrComment("editing range is >= 0 <= 1"))
		UPROPERTY(mInt0)
		UPROPERTY(mUneditableNumber, AttrUneditable());
		UPROPERTY(mBool0)
		UPROPERTY(mName)
		UPROPERTY(mString)
		UPROPERTY(mStringMultiLine, AttrMultiLine())
		UPROPERTY(mVec2)
		UPROPERTY(mVec3)
		UPROPERTY(mVec4)
		UPROPERTY(mColor)
		UPROPERTY(mColor32)
		UPROPERTY(mAssetPtr0)
		UPROPERTY(mArrayOfVec3)
		UPROPERTY(mArrayOfName)
		UPROPERTY(mSubClass)
		UPROPERTY(mChildObject)
	UCLASS_END_IMPL(ADebugAsset)
};