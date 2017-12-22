#pragma once

#include "Asset.h"
#include "../Core/Vector.h"

namespace UEngine
{
	struct DebugTestStructFoo
	{
		UCLASS(DebugTestStructFoo)

		Name mTag = "Tag";
		Vec3 mPosition = Vec3(0.0f);
	};

	struct DebugTestStructBar : DebugTestStructFoo
	{
		UCLASS(DebugTestStructBar, DebugTestStructFoo)

		Color mColor = Color::YELLOW;
		TObjectPtr<Asset> mAsset;
	};

	//////////////////////////////////////////////////////////////////////////
	//this asset is just for testing
	class UENGINE_API ADebugAsset : public Asset
	{
		UCLASS(ADebugAsset, Asset)

		float mFloat0 = 0;
		float mFloatMinMax = 0;
		int mInt0 = 0;
		int mUneditableNumber = 100;
		bool mBool0 = false;
		Name mName = "Name";
		String mString;
		String mStringMultiLine;
		Vec2 mVec2 = Vec2(0.0f);
		Vec3 mVec3 = Vec3(0.0f);
		Vec3 mVec4 = Vec4(0.0f);
		Color mColor = Color::RED;
		Color32 mColor32 = Color32::GREEN;
		TObjectPtr<Asset> mAssetPtr0;
		TArray<Vec3> mArrayOfVec3;
		TArray<Name> mArrayOfName;
		TSubClass<Object> mSubClass;


		Object* mChildObject = nullptr;
	};
};