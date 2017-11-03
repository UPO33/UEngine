#pragma once
#include "../Core/All.h"

namespace UCoreTest
{
	using namespace UCore;

	struct TestStructBase
	{
		UCLASS(TestStructBase)
	public:
		bool						mBool = false;
		char						mChar = 128;
		unsigned char				mUChar = 128;
		short						mShort = 256;
		unsigned short				mUShort = 256;
		int							mInt = 512;
		unsigned int				mUInt = 512;
		int64						mInt64 = 1024;
		uint64						mUInt64 = 1024;
		Vec2						mVec2 = Vec2(1,2);
		Vec3						mVec3 = Vec3(1,2,3);
		Vec4						mVec4 = Vec4(1,2,3,4);
		Color						mColor = Color(1,0,0,1);
		Color32						mColor32 = Color32(0,255,0,255);
		String						mString = String("String");
		Name						mName = Name("Name");
		TArray<Vec3>				mTArrayVec3;
		TArray<int>					mTArrayInt;
		TArray<String>				mTArrayString;
		Object*						mObjectStar = nullptr;
		TSubClass<Object>			mSubClass;
		TObjectPtr<Object>			mObjectPtr;
		TArray<TSubClass<Object>>	mSubClassArray;
		TArray<TObjectPtr<Object>>	mObjectPtrArray;

		TestStructBase();
	};

	struct TestStruct1 : public TestStructBase
	{
		UCLASS(TestStruct1, TestStructBase)

		int		mStruct1Int;
		bool	mStruct2BoolArray[8];
	};



	enum ETestEnum
	{
		ETE_Element0, ETE_Element1, ETE_Element2
	};
	enum ETestFlag
	{
		ETF_Flag1 = 1,
		ETF_Flag2 = 2,
		ETF_Flag3 = 4,
		ETF_All = ETF_Flag1 | ETF_Flag2 | ETF_Flag3,
	};


	void MetaTestMain();
};