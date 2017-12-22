
#include "../Core/Vector.h"
#include "../Core/Log.h"
#include "../Engine/Base.h"
#include "../Engine/ObjectArchive.h"
#include "../Engine/Asset.h"
#include "../Engine/Entity.h"
#include "../Core/ByteSerializer.h"
#include "../Core/MetaArray.h"
#include "../Core/Hash.h"

using namespace UEngine;


struct TestPlayerInfo
{
	UCLASS(TestPlayerInfo)

	Name		mName;
	int			mScore = 0;
	class TestEntity* mOwner = nullptr;
};

UCLASS_BEGIN_IMPL(TestPlayerInfo)
UPROPERTY(mName)
UPROPERTY(mScore)
UPROPERTY(mOwner)
UCLASS_END_IMPL(TestPlayerInfo)

class TestEntity : public Object
{
	UCLASS(TestEntity, Object)

	Name													mName;
	Name													mTag;
	Vec3													mPosition = Vec3(0.0f);
	float													mScale = 0;
	Vec4													mVec4 = Vec4(0.0f);
	int														mInt = 0;
	TArray<int>												mNormalIntArray;
	TArray<Vec3, TArrayAllocStack<Vec3, 33>>				mVec3Array;
	TArray<TestPlayerInfo>									mPlayerInfos;
	Asset*													mAssetPtr = nullptr;
	Object*												mPtrOutside = nullptr;
	Object*												mPtrInside = nullptr;

	virtual void PrintDbg(int indent = 0)
	{
		
		ULOG_MESSAGE("% mName: %  mTag: % mPosition: % mScale: %", LogIndent(indent), mName, mTag, mPosition, mScale);
		ULOG_MESSAGE("mVec4: %   mInt: %", mVec4, mInt);
		for (size_t i = 0; i < mNormalIntArray.Length(); i++)
		{
			ULOG_MESSAGE("mNormalIntArray[%] = %", i, mNormalIntArray[i]);
		}
		for (size_t i = 0; i < mVec3Array.Length(); i++)
		{
			ULOG_MESSAGE("mVec3Array[%] = %", i, mVec3Array[i]);
		}
		for (size_t i = 0; i < mPlayerInfos.Length(); i++)
		{
			ULOG_MESSAGE("mPlayerInfos[%] = { %, %, %)",
				i, mPlayerInfos[i].mName, mPlayerInfos[i].mScore, mPlayerInfos[i].mOwner);
		}

		ULOG_MESSAGE("mPtrInside % mPtrOutside %", (size_t)mPtrInside, (size_t)mPtrOutside);

		this->ForEachChild(false, false, [&](Object* child) {
			if (child->Cast<TestEntity>())
				child->Cast<TestEntity>()->PrintDbg(indent + 1);
		});
	}
	virtual void Set0()
	{
		this->ForEachChild(false, false, [&](Object* child) {
			if (child->Cast<TestEntity>())
				child->Cast<TestEntity>()->Set0();
		});

		mName = "Name";
		mTag = "Tag";
		mPosition = Vec3(1, 2, 3);
		mScale = 4;
		mVec4 = Vec4(3, 4, 5, 0);
		mInt = 6;

		mNormalIntArray.Add(1);
		mNormalIntArray.Add(2);
		mNormalIntArray.Add(3);

		mVec3Array.Add(Vec3(10.0f));
		mVec3Array.Add(Vec3(20.0f));
		mVec3Array.Add(Vec3(30.0f));

		mPlayerInfos.Add(TestPlayerInfo{ "player0", 100, this });
		mPlayerInfos.Add(TestPlayerInfo{ "player1", 200, this });

		mPtrOutside = NewObject<Object>();
		mPtrOutside->SetObjectDebugStr("PtrOutSide");
		mPtrInside = NewObject<Object>(Object::GetClassStatic(), this);
		mPtrInside->SetObjectDebugStr("PtrInside");
	}
};

UCLASS_BEGIN_IMPL(TestEntity)
UPROPERTY(mName)
UPROPERTY(mTag)
UPROPERTY(mPosition)
UPROPERTY(mScale)
UPROPERTY(mVec4)
UPROPERTY(mInt)
UPROPERTY(mNormalIntArray)
UPROPERTY(mVec3Array)
UPROPERTY(mPlayerInfos)
UPROPERTY(mPtrOutside)
UPROPERTY(mPtrInside)
UCLASS_END_IMPL(TestEntity)



Object* UCreateTestPrefab()
{
	auto root = NewObject<TestEntity>();
	{
		//TestEntity* a = NewObject<TestEntity2>(TestEntity2::GetClassStatic(), root);
// 		{
// 			NewObject<TestEntity>(TestEntity3::GetClassStatic(), a);
// 			NewObject<TestEntity>(TestEntity3::GetClassStatic(), a);
// 			NewObject<TestEntity>(TestEntity3::GetClassStatic(), a);
// 		}
// 		NewObject<TestEntity>(TestEntity2::GetClassStatic(), root);
// 		NewObject<TestEntity>(TestEntity2::GetClassStatic(), root);
	}

	return root;
}

// void ClassGetObjectProperties(const ClassInfo* pClass, const void* pInstance, TArray<Object*>& outObjects)
// {
// 	for (const ClassInfo* pClassChain : pClass->GetClassChain())
// 	{
// 		for (const PropertyInfo* pProperty : pClassChain->GetProperties())
// 		{
// 			EMetaType type = pProperty->GetType();
// 			size_t count = 1;
// 			const void* pValues = pProperty->Map(pInstance);
// 			const ClassInfo* classType = Cast<ClassInfo>(pProperty->GetSelf().GetPtr());
// 
// 			bool isTArrayProperty = pProperty->GetType() == EMetaType::EPT_TArray;
// 
// 			if (isTArrayProperty)
// 			{
// 				MetaArrayView arrayView((void*)pValues, pProperty->GetSelf());
// 				count = arrayView.Length();
// 				type = pProperty->GetArg0().GetType();
// 				pValues = arrayView.Elements();
// 				classType = Cast<ClassInfo>(pProperty->GetArg0().GetPtr());
// 			}
// 
// 			for (size_t iIndex = 0; iIndex < count; iIndex++)
// 			{
// 				if (type == EMetaType::EPT_TObjectPtr || type == EMetaType::EPT_ObjectPoniter)
// 				{
// 					Object* obj = nullptr;
// 
// 					if(type == EMetaType::EPT_TObjectPtr)
// 					{
// 						ObjectPtr& prpValue = ((ObjectPtr*)pValues)[iIndex];
// 						obj = prpValue.Get();
// 					}
// 					else
// 					{
// 						Object*& prpValue = ((Object**)pValues)[iIndex];
// 						obj = prpValue;
// 					}
// 
// 					if(obj)
// 					{
// 						if (!outObjects.HasElement(obj))
// 						{
// 							outObjects.Add(obj);
// 							ClassGetObjectProperties(obj->GetClass(), obj, outObjects);
// 						}
// 					}
// 				}
// 				else if (type == EMetaType::EPT_Class)
// 				{
// 					if (classType)
// 					{
// 						void* p = ((byte*)pValues) + (classType->GetSize() * iIndex);
// 						ClassGetObjectProperties(pClassChain, p, outObjects);
// 					}
// 				}
// 			}
// 			
// 		}
// 	}
// 
// }

void UExtractObjectTree(Object* obj, TArray<Object*>& out)
{
	out.Add(obj);
	
	obj->ForEachChild(true, true, [&](Object* child){
		out.Add(child);
	});

}
bool UObjectHierarchyClassIsEqual( Object* a,  Object* b)
{
	TArray<const ClassInfo*> classesOfA;
	TArray<const ClassInfo*> classesOfB;

	classesOfA.Add(a->GetClass());
	classesOfB.Add(b->GetClass());
	
	a->ForEachChild(true, true, [&](Object* obj) {
		UASSERT(obj);
		classesOfA.Add(obj->GetClass());
	});
	b->ForEachChild(true, true, [&](Object* obj) {
		UASSERT(obj);
		classesOfB.Add(obj->GetClass());
	});

	return classesOfA.IsEqual(classesOfB, true);
}

//////////////////////////////////////////////////////////////////////////
class TestCloneObject : public Object
{
	UCLASS(TestCloneObject, Object)

	Name mName = "naaaame";
	Vec3 mDirection = Vec3(0.0f);

	virtual void DebugPrint()
	{
		ULOG_MESSAGE("Name %", mName);
	}
	virtual void OnPostClone()
	{
		this->mName = nullptr;
	}
};
UCLASS_BEGIN_IMPL(TestCloneObject)
UCLASS_END_IMPL(TestCloneObject)

void UTestCloneObject()
{
	auto root = NewObject<TestCloneObject>();
	auto child0 = NewObject<TestCloneObject>(TestCloneObject::GetClassStatic(), root);
	auto child1 = NewObject<TestCloneObject>(TestCloneObject::GetClassStatic(), root);
	auto child00 = NewObject<TestCloneObject>(TestCloneObject::GetClassStatic(), child0);
	auto child10 = NewObject<TestCloneObject>(TestCloneObject::GetClassStatic(), child1);

	root->mName = "Root";
	child1->mName = "Child1";
	child0->mName = "child0";
	child00->mName = "child00";
	child10->mName = "child10";


	Object* cloned = root->Clone(false);
	ULOG_MESSAGE("");
}

int main_(int argc, char** argv)
{
	{
		UTestCloneObject();
	}
	TestEntity* objToSave = NewObject<TestEntity>();
	objToSave->SetObjectDebugStr("root");
	objToSave->Set0();
	objToSave->PrintDbg();

	//UPrintObjectClassHierarchy(objToSave);

	ByteSerializer ser0;
	UASSERT(USaveArchive(objToSave, ser0, true, false));
	{
		ByteDeserializer deser0 = ByteDeserializer(ser0.GetData(), ser0.GetCurPos());
		Object* objLoaded = ULoadArchive(deser0);
		objLoaded->CastSure<TestEntity>()->PrintDbg();
		//UPrintObjectClassHierarchy(objLoaded);
		UASSERT(UObjectHierarchyClassIsEqual(objToSave, objLoaded));

		{
			ByteSerializer ser1;
			UASSERT(USaveArchive(objLoaded, ser1, true, false));
			UASSERT(
				CRC32::HashBuffer(ser0.GetData(), ser0.GetCurPos())
				== CRC32::HashBuffer(ser1.GetData(), ser1.GetCurPos()));

		}
	}

	return 0;
}