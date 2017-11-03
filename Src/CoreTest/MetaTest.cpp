#include "MetaTest.h"




#include "../Core/json.h"

#include "../Core/Jzon.h"

#include "../Core/Config.h"

namespace UCoreTest
{
	UCLASS_BEGIN_IMPL(TestStructBase)
		UPROPERTY(mBool)
		UPROPERTY(mChar)
		UPROPERTY(mUChar)
		UPROPERTY(mShort)
		UPROPERTY(mUShort)
		UPROPERTY(mInt)
		UPROPERTY(mUInt)
		UPROPERTY(mInt64)
		UPROPERTY(mUInt64)
		UPROPERTY(mVec2)
		UPROPERTY(mVec3)
		UPROPERTY(mVec4)
		UPROPERTY(mColor)
		UPROPERTY(mColor32)
		UPROPERTY(mString)
		UPROPERTY(mName)
		UPROPERTY(mTArrayVec3)
		UPROPERTY(mTArrayInt)
		UPROPERTY(mTArrayString)
		UPROPERTY(mObjectStar)
		UPROPERTY(mSubClass)
		UPROPERTY(mObjectPtr)
		UPROPERTY(mSubClassArray)
		UPROPERTY(mObjectPtrArray)
		UCLASS_END_IMPL(TestStructBase)

	TestStructBase::TestStructBase()
	{
		mTArrayVec3.Add(Vec3(1));
		mTArrayVec3.Add(Vec3(2));
		mTArrayVec3.Add(Vec3(3));
		mTArrayString.Add("str0");
		mTArrayString.Add("Str1");
		mTArrayInt.Add(1);
		mTArrayInt.Add(2);

	}

	UCLASS_BEGIN_IMPL(TestStruct1)
		UPROPERTY(mStruct1Int)
	UCLASS_END_IMPL(TestStruct1)

};

namespace UCoreTest
{


// 	void JSONReadProperty(const PropertyInfo* pProperty, size_t arrayElementIndex, void* mappedValue, const Jzon::Node& inJS)
// 	{
// 		std::string propertyName = pProperty->GetName().CStr();
// 		EFieldType type = pProperty->GetSelf().GetType();
// 		if (arrayElementIndex != INVALID_INDEX)
// 		{
// 			type = pProperty->GetArg0().GetType();
// 		}
// 
// 		Jzon::Node node = inJS.get(propertyName);
// 		if (node.isValid())
// 		{
// 			switch (type)
// 			{
// 			case EFieldType::EPT_bool:
// 				node.toBool();
// 			}
// 		}
// 
// 	}
// 
// 	void JSONReadClass(const char* inJSONString, const ClassInfo* pClass, void* pInstance)
// 	{
// 		UASSERT(pClass && pInstance);
// 
// 		Jzon::Parser parser;
// 		Jzon::Node node = parser.parseString(inJSONString);
// 
// 		for (const ClassInfo* classIter : pClass->GetClassChain())
// 		{
// 			for (const PropertyInfo* propertyIter : classIter->GetProperties())
// 			{
// 				JSONReadProperty(propertyIter, INVALID_INDEX, propertyIter->Map(pInstance), node);
// 			}
// 		}
// 	}


	void MetaTestMain()
	{
		
	}
};