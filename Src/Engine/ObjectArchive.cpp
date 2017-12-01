#pragma once

#include "ObjectArchive.h"

#include "../Core/Meta.h"
#include "../Core/ByteSerializer.h"
#include "../Core/MetaArray.h"
#include "../Core/Compression.h"


#include "Asset.h"
#include "Version.h"

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	void UCollectNameProperties(const TArray<Object*>& inObjects, TArray<Name*>& outPointerToProperties)
	{
		for (Object* pObj : inObjects)
		{
			if (pObj == nullptr) continue;

			for (const ClassInfo* pClass : pObj->GetClass()->GetClassChain())
			{
				for (const PropertyInfo* pProperty : pClass->GetProperties())
				{
					//is Name property?
					if (pProperty->GetSelf().GetType() == EMetaType::EPT_Class
						&& pProperty->GetSelf().GetPtr() == Name::GetClassStatic())
					{
						outPointerToProperties.Add((Name*)pProperty->Map(pObj));
					}
					//is TArray<Name> ?
					else if (pProperty->GetSelf().GetType() == EMetaType::EPT_TArray
						&& pProperty->GetArg0().GetType() == EMetaType::EPT_Class
						&& pProperty->GetArg0().GetPtr() == Name::GetClassStatic())
					{
						MetaArrayView arrayView = MetaArrayView(pProperty->Map(pObj), pProperty->GetSelf());
						for (size_t iElement = 0; iElement < arrayView.Length(); iElement++)
						{
							outPointerToProperties.Add((Name*)arrayView.GetElementAt(iElement));
						}
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	struct ArchiveHeadData
	{
		char mID[3] = { 'U', 'O', 'A' };
		char mVersionMajor = ENGINE_VERSION_MAJOR;
		char mVersionMinor = ENGINE_VERSION_MINOR;
		bool mCooked = false;
		bool mCompress = false;
		uint32 mSize = 0;

		void Write(ByteSerializer& ser)
		{
			ser.Bytes(mID, sizeof(mID));
			ser << (mVersionMajor);
			ser << (mVersionMinor);
			ser << (mCooked);
			ser << (mCompress);
			ser << (mSize);
		}
		void Read(ByteDeserializer& ser)
		{
			ser.Bytes(mID, sizeof(mID));
			ser >> (mVersionMajor);
			ser >> (mVersionMinor);
			ser >> (mCooked);
			ser >> (mCompress);
			ser >> (mSize);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct ObjectSerializer
	{
		bool Save(const Object* root, ByteSerializer& ser)
		{
			ULOG_FATAL("#TODO not implemented yet");
			return false;
		}
		Object* Load(ByteDeserializer& ser)
		{
			ULOG_FATAL("#TODO not implemented yet");
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct ObjectSerializerCooked
	{
		bool Save(const Object* rootObject, ByteSerializer& ser)
		{
			if (rootObject == nullptr) return false;

			
			//collecting objects
			{
				mObjects.RemoveAll();

				mObjects.Add((Object*)rootObject);

				((Object*)rootObject)->ForEachChild(true, true, [this](Object* child) {
					mObjects.Add(child);
				});

				//loging 
				{
					for (Object* pObj : mObjects)
					{
						ULOG_MESSAGE("object %", pObj->GetObjectDebugStr() ? pObj->GetObjectDebugStr() : "");
					}
				}
				ULOG_MESSAGE("% object collected", mObjects.Length());
			}

			//estimating the capacity required for serializer
			{
				size_t estimate = 0;
				for (Object* pObj : mObjects)
				{
					estimate += pObj->GetClass()->GetSize();
				}
				ser.IncCapacity(estimate);
			}

			WObjects(ser);
			return true;
		}
		Object* Load(ByteDeserializer& ser)
		{
			RObjects(ser);

			PerformObjectPointerPhase();
			PerformAssetPointerPhase();

			mObjects[0]->OnPostLoad();

			return mObjects[0];
		}

	private:
		//arrays of objects that is written or read, the first element is root object
		TArray<Object*>	mObjects;

		uint32 IndexOfObject(const Object* pObject)
		{
			return (uint32)mObjects.Find((Object*)pObject);
		}
		static void WClassName(const ClassInfo* pClass, ByteSerializer& ser)
		{
			NameHashT hash = pClass->GetName().GetHash();
			ser << hash;
		}
		static const ClassInfo* RClassName(ByteDeserializer& ser)
		{
			NameHashT hash = 0;
			ser >> hash;
			return GMetaSys()->FindClass(hash);
		}

		using SubClass = TSubClass<Object>;

	

		//////////////////////////////////////////////////////////////////////////
		void WObjects(ByteSerializer& ser)
		{
			uint32 numObject = (uint32)mObjects.Length();
			ser << numObject;

			//writing class name hash of objects
			for(uint32 iObj = 0; iObj < numObject; iObj++)
			{
				WClassName(mObjects[iObj]->GetClass(), ser);
			}
			//writing parent index of objects
			for (uint32 iObj = 0; iObj < numObject; iObj++)
			{
				uint32 parentIndex = ~uint32(0);
				if (iObj != 0)
				{
					parentIndex = (uint32)mObjects.Find(mObjects[iObj]->GetObjectParent());
					UASSERT(parentIndex != ~uint32(0));
				}
				ser << parentIndex;
			}
			//writing properties of objects
			for (uint32 iObj = 0; iObj < numObject; iObj++)
			{
				WObject(mObjects[iObj], ser);
			}
		}

		/*
			uint32 numObject, classNameHash[numObject], uint32 parentIndex[numObject]
		*/
		//////////////////////////////////////////////////////////////////////////
		void RObjects(ByteDeserializer& ser)
		{
			TArray<const ClassInfo*> objectsClass;
			TArray<uint32>			 ObjectsParentIndex;

			uint32 numObject = 0;
			ser >> numObject;

			mObjects.RemoveAll();
			mObjects.AddDefault(numObject);

			objectsClass.AddDefault(numObject);
			ObjectsParentIndex.AddDefault(numObject);

			//reading classes name
			for (uint32 iObj = 0; iObj < numObject; iObj++)
			{
				objectsClass[iObj] = RClassName(ser);
				UASSERT(objectsClass[iObj]);
				UASSERT(objectsClass[iObj]->IsObjectClass());
			}
			//reading parent indices
			ser.Bytes(ObjectsParentIndex.Elements(), numObject * sizeof(uint32));

			//newing and reading objects 
			for (uint32 iObj = 0; iObj < numObject; iObj++)
			{
				Object* parent = nullptr;
				const ClassInfo* objectClass = objectsClass[iObj];

				if (iObj != 0) //first object is root and must not have parent
				{
					UASSERT(ObjectsParentIndex[iObj] != INVALID_INDEX);
					parent = mObjects[ObjectsParentIndex[iObj]];
					UASSERT(parent);
				}

				//allocating and initializing object
				{
					Object* newObject = ObjectUtility::NewDefault(objectClass, parent, false);

					mObjects[iObj] = newObject;
					RObject(newObject, ser);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void WObject(const Object* pObject, ByteSerializer& ser)
		{
			((Object*)pObject)->OnMetaSerialize(ser);

			const ClassInfo* pClass = pObject->GetClass();

			for (const ClassInfo* pClassIter : pClass->GetClassChain())
			{
				for (const PropertyInfo* pProperty : pClassIter->GetProperties())
				{
					if (pProperty->GetAttribute<AttrVolatile>() == nullptr)	//is not volatile?
					{
						WProperty(pProperty, pProperty->Map(pObject), ser);
					}
				}
			}


		}
		//////////////////////////////////////////////////////////////////////////
		void RObject(Object* pObject, ByteDeserializer& ser)
		{
			pObject->OnMetaDeserialize(ser);

			const ClassInfo* pClass = pObject->GetClass();

			for (const ClassInfo* pClassIter : pClass->GetClassChain())
			{
				for (const PropertyInfo* pProperty : pClassIter->GetProperties())
				{
					if (pProperty->GetAttribute<AttrVolatile>() == nullptr)	//is not volatile?
					{
						RProperty(pProperty, pProperty->Map(pObject), ser);
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void WClass(const ClassInfo* pClass, const void* pInstances, size_t count, ByteSerializer& ser)
		{
			for (size_t iInstance = 0; iInstance < count; iInstance++)
			{
				void* instance = reinterpret_cast<void*>(((size_t)pInstances) + pClass->GetSize() * iInstance);
				
				//get the top most MetaSerialize function and call it if any
				//#Note: MetaSerialize must call its parent MetaSerialize
				{
					const ClassInfo* pClassMetaSerialize = nullptr;
					for (const ClassInfo* pClassIter : pClass->GetClassChain())
					{
						if (pClassIter->HasMetaSerialize())
							pClassMetaSerialize = pClassIter;
					}
					if (pClassMetaSerialize)
						pClassMetaSerialize->CallMetaSerialize(instance, ser);
				}
				//writing properties
				for (const ClassInfo* pClassIter : pClass->GetClassChain())
				{
					for (const PropertyInfo* pProperty : pClassIter->GetProperties())
					{
						if (pProperty->GetAttribute<AttrVolatile>() == nullptr)	//is not volatile?
						{
							WProperty(pProperty, pProperty->Map(instance), ser);
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void RClass(const ClassInfo* pClass, void* pInstances, size_t count, ByteDeserializer& ser)
		{
			for (size_t iInstance = 0; iInstance < count; iInstance++)
			{
				void* instance = reinterpret_cast<void*>(((size_t)pInstances) + pClass->GetSize() * iInstance);

				//get the top most MetaDeserialize function and call it if any
				//#Note: MetaDeserialize must call its parent MetaDeserialize
				{
					const ClassInfo* pClassMetaDeserialize = nullptr;
					for (const ClassInfo* pClassIter : pClass->GetClassChain())
					{
						if (pClassIter->HasMetaDeserialize())
							pClassMetaDeserialize = pClassIter;
					}
					if (pClassMetaDeserialize)
						pClassMetaDeserialize->CallMetaDeserialize(instance, ser);
				}
				//reading properties
				for (const ClassInfo* pClassIter : pClass->GetClassChain())
				{
					for (const PropertyInfo* pProperty : pClassIter->GetProperties())
					{
						if (pProperty->GetAttribute<AttrVolatile>() == nullptr)	//is not volatile?
						{
							RProperty(pProperty, pProperty->Map(instance), ser);
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//write a property of a class, if property is TArray, a uin32 as the length of array is written then the elements
		//@pProperty		the intended property
		//@pMappedVlaue		pointer to the value of property, for instance if property is int it means int*
		//@stream			the stream to write data to
		//////////////////////////////////////////////////////////////////////////
		void WProperty(const PropertyInfo* pProperty, const void* pMappedValue, ByteSerializer& ser)
		{
			//type of the property, if property is array its the type of its array element
			EMetaType type = pProperty->GetType();
			//in cause that 'type' is class this is the ClassInfo of it
			const ClassInfo* typeClass = Cast<ClassInfo>(pProperty->GetTypePtr());
			//number of element we write, if not array its one
			size_t count = 1;
			//pointers to the values that must be written for instance of 
			const void* pValues = pMappedValue;

			const bool isTArrayProperty = pProperty->GetType() == EMetaType::EPT_TArray;

			if (isTArrayProperty)
			{
				MetaArrayView arrayView = MetaArrayView((void*)pMappedValue, pProperty->GetArg0());

				uint32 arrayLength = (uint32)arrayView.Length();
				ser << arrayLength;

				if (arrayLength == 0)
					return;

				type = pProperty->GetArg0().GetType();
				typeClass = Cast<ClassInfo>(pProperty->GetArg0().GetPtr());
				count = arrayView.Length();
				pValues = arrayView.Elements();
			}




			switch (type)
			{
			case EMetaType::EPT_Unknown:
				ULOG_FATAL("");
				break;
			case EMetaType::EPT_bool:
			case EMetaType::EPT_int8:
			case EMetaType::EPT_uint8:
			case EMetaType::EPT_int16:
			case EMetaType::EPT_uint16:
			case EMetaType::EPT_int32:
			case EMetaType::EPT_uint32:
			case EMetaType::EPT_int64:
			case EMetaType::EPT_uint64:
			case EMetaType::EPT_float:
			case EMetaType::EPT_double:
			case EMetaType::EPT_enum:
			{
				size_t typeSize = GetPropertyTypeSize(type);
				ser.Bytes((void*)pValues, typeSize * count);
				break;
			}
			case EMetaType::EPT_TSubClass:
			{
				WTSubClass(pValues, count, ser);
				break;
			}
			case EMetaType::EPT_TObjectPtr:
			{
				const ClassInfo* objectPopertyClass = nullptr;
				if (isTArrayProperty) //property is TArray<TObjectPtr<ObjectClass>>
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetArg0Arg0().GetPtr());
				else //property is TObjectPtr<Class>
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetArg0().GetPtr());

				WTObjectPtrOrObjectPointer(objectPopertyClass, pValues, count, ser, true);
				break;
			}
			case EMetaType::EPT_ObjectPoniter:
			{
				const ClassInfo* objectPopertyClass = nullptr;
				if (isTArrayProperty) //property is TArray<Object*>
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetArg0().GetPtr());
				else //property is Object*
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetSelf().GetPtr());

				WTObjectPtrOrObjectPointer(objectPopertyClass, pValues, count, ser, false);
				break;
			};
			case EMetaType::EPT_TArray:
			{
				ULOG_FATAL("TArray as TArray's element");
				break;
			}
			case EMetaType::EPT_Class:
			{
				UASSERT(typeClass);
				WClass(typeClass, pValues, count, ser);
				break;
			}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void RProperty(const PropertyInfo* pProperty, void* pMappedValue, ByteDeserializer& ser)
		{
			EMetaType type = pProperty->GetType();
			const ClassInfo* typeClass = Cast<ClassInfo>(pProperty->GetTypePtr());
			size_t count = 1;
			void* pValues = pMappedValue;
			bool isTArrayProperty = pProperty->GetType() == EMetaType::EPT_TArray;

			if (isTArrayProperty)
			{
				MetaArrayView arrayView = MetaArrayView(pMappedValue, pProperty->GetArg0());

				uint32 arrayLength = 0;
				ser >> arrayLength;

				if (arrayLength == 0)
					return;

				arrayView.RemoveAll();
				arrayView.AddDefault((size_t)arrayLength);

				type = pProperty->GetArg0().GetType();
				typeClass = Cast<ClassInfo>(pProperty->GetArg0().GetPtr());
				count = (size_t)arrayLength;
				pValues = arrayView.Elements();
			}




			switch (type)
			{
			case EMetaType::EPT_Unknown:
				ULOG_FATAL("");
				break;
			case EMetaType::EPT_bool:
			case EMetaType::EPT_int8:
			case EMetaType::EPT_uint8:
			case EMetaType::EPT_int16:
			case EMetaType::EPT_uint16:
			case EMetaType::EPT_int32:
			case EMetaType::EPT_uint32:
			case EMetaType::EPT_int64:
			case EMetaType::EPT_uint64:
			case EMetaType::EPT_float:
			case EMetaType::EPT_double:
			case EMetaType::EPT_enum:
			{
				size_t typeSize = GetPropertyTypeSize(type);
				ser.Bytes(pValues, typeSize * count);
				break;
			}
			case EMetaType::EPT_TSubClass:
			{
				RTSubClass(pValues, count, ser);
				break;
			}
			case EMetaType::EPT_TObjectPtr:
			{
				const ClassInfo* objectPopertyClass = nullptr;
				if (isTArrayProperty) //property is TArray<TObjectPtr<ObjectClass>>
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetArg0Arg0().GetPtr());
				else //property is TObjectPtr<Class>
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetArg0().GetPtr());

				RTObjectPtrOrObjectPointer(objectPopertyClass, pValues, count, ser, true);
				break;
			}
			case EMetaType::EPT_ObjectPoniter:
			{
				const ClassInfo* objectPopertyClass = nullptr;
				if (isTArrayProperty) //property is TArray<Object*>
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetArg0().GetPtr());
				else //property is Object*
					objectPopertyClass = CastSure<const ClassInfo>(pProperty->GetSelf().GetPtr());

				RTObjectPtrOrObjectPointer(objectPopertyClass, pValues, count, ser, false);
				break;
			};
			case EMetaType::EPT_TArray:
			{
				ULOG_FATAL("TArray as TArray's element");
				break;
			}
			case EMetaType::EPT_Class:
			{
				UASSERT(typeClass);
				RClass(typeClass, pValues, count, ser);
				break;
			}
			}
		}


		//////////////////////////////////////////////////////////////////////////
		//@bTObjectPtr	whether @pItems is array of TObjectPtr or Object*
		//if object is not asset the index of it in mObjects is written, if object is null INVALID_INDEX is written
		//if object is asset the ID of asset is written
		void WTObjectPtrOrObjectPointer(const ClassInfo* pObjectClass, const void* pItems, size_t count, ByteSerializer& ser, bool bTObjectPtr)
		{
			auto asTObjectPtr = (TObjectPtr<Object>*)(pItems);
			auto asObjectStar = (Object**)(pItems);

			for (size_t i = 0; i < count; i++)
			{
				Object* pObject = nullptr;

				if (bTObjectPtr)
					pObject = asTObjectPtr[i].Get();
				else
					pObject = asObjectStar[i];

				if (pObjectClass->IsBaseOf<Asset>()) //is asset?
				{
					if (pObject)
					{
						Asset* pAsset = UCastSure<Asset>(pObject);
						pAsset->GetID().MetaSerialize(ser);
					}
					else
					{
						AssetID().MetaSerialize(ser);
					}
				}
				else
				{
					uint32 index = ~uint32(0);
					if (pObject)
						index = (uint32)mObjects.Find(pObject);

					ser << index;
				}
			}
		}
		//this struct is used to store the required data for a Object* or TObjectPtr property so that
		//at the end of loading and constructing the objects, these properties must be initialized, I call it Pointer Phase
		struct SObjectProperty
		{
			SObjectProperty(void* ptr, bool bTObjectPtr, uint32 index)
				: mProperty(ptr), mIsTObjectPtr(bTObjectPtr), mIndex(index) {}

			union
			{
				void*					mProperty;
				Object**				mPropertyObject;	//pointer to a Object* property, valid if mIsTObjectPtr == false
				TObjectPtr<Object>*		mPropertyTObjectPtr; //pointer to a TObjectPtr property, valid if mIsTObjectPtr == true
			};
			bool					mIsTObjectPtr;
			uint32					mIndex;	//the index for mObject array to get the object
		};
		//same as above but for assets
		struct SAssetProperty
		{
			SAssetProperty(void* ptr, bool bTObjectPtr, AssetID assetId) :
				mProperty(ptr), mIsTObjectPtr(bTObjectPtr), mAssetID(assetId)
			{
			}

			union
			{
				void*					mProperty;
				Asset**					mPropertyObject;	//pointer to a Object* property, valid if mIsTObjectPtr == false
				TObjectPtr<Asset>*		mPropertyTObjectPtr; //pointer to a TObjectPtr property, valid if mIsTObjectPtr == true
			};

			bool		mIsTObjectPtr;
			AssetID		mAssetID;
		};

		TArray<SObjectProperty>		mPointerPhaseObjectPtrProperties;
		TArray<SAssetProperty>		mPointerPhaseAssetPtrProperties;

		//////////////////////////////////////////////////////////////////////////
		//initializes the Object* and TObjectPtr properties of loaded objects, non-assets only
		void PerformObjectPointerPhase()
		{
			for (const SObjectProperty& iter : mPointerPhaseObjectPtrProperties)
			{
				Object* pObjectToAssign = nullptr;
				
				if(iter.mIndex != ~uint32(0))
					pObjectToAssign = mObjects[iter.mIndex];

				if (iter.mIsTObjectPtr)
					*iter.mPropertyTObjectPtr = pObjectToAssign;
				else
					*iter.mPropertyObject = pObjectToAssign;
			}

			mPointerPhaseObjectPtrProperties.RemoveAll();
		}

		//////////////////////////////////////////////////////////////////////////
		//same as above but for assets
		void PerformAssetPointerPhase()
		{
			for (const SAssetProperty& iter : mPointerPhaseAssetPtrProperties)
			{
				Asset*	pAssetToAssign = GAssetMgr()->LoadAsset(iter.mAssetID);

				if (iter.mIsTObjectPtr)
					*iter.mPropertyTObjectPtr = pAssetToAssign;
				else
					*iter.mPropertyObject = pAssetToAssign;
			}

			mPointerPhaseAssetPtrProperties.RemoveAll();
		}
		//////////////////////////////////////////////////////////////////////////
		void RTObjectPtrOrObjectPointer(const ClassInfo* pObjectClass, void* pItems, size_t count, ByteDeserializer& ser, bool bTObjectPtr)
		{
			//casting as array of Object* and TObjectPtr
			auto asTObjectPtr = (TObjectPtr<Object>*)(pItems);
			auto asObjectStar = (Object**)(pItems);

			for (size_t i = 0; i < count; i++)
			{
				Object* pObject = nullptr;

				if (bTObjectPtr)
					pObject = asTObjectPtr[i].Get();
				else
					pObject = asObjectStar[i];

				if (pObjectClass->IsBaseOf<Asset>()) //is asset?
				{
					AssetID assetID;
					assetID.MetaDeserialize(ser);

					if(bTObjectPtr)
						mPointerPhaseAssetPtrProperties.Add(SAssetProperty((TObjectPtr<Asset>*)(asObjectStar + i), bTObjectPtr, assetID));
					else
						mPointerPhaseAssetPtrProperties.Add(SAssetProperty((Asset**)(asObjectStar + i), bTObjectPtr, assetID));
				}
				else
				{
					uint32 index = ~uint32(0);
					ser >> index;

					//even invalid index is added to array, pointer phase must zero it
					if (bTObjectPtr)
						mPointerPhaseObjectPtrProperties.Add(SObjectProperty(asTObjectPtr + i, true, index));
					else
						mPointerPhaseObjectPtrProperties.Add(SObjectProperty(asObjectStar + i, false, index));

				}
			}
		}


		//////////////////////////////////////////////////////////////////////////
		static void WTSubClass(const void* arrayOfTSubClass, size_t count, ByteSerializer& ser)
		{
			const ClassInfo** subClasses = (const ClassInfo**)arrayOfTSubClass;

			for (size_t i = 0; i < count; i++)
			{
				NameHashT hash = Name().GetHash();
				if (const ClassInfo* pClass = subClasses[i])
					hash = pClass->GetName().GetHash();

				ser << hash;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static void RTSubClass(void* arrayOfTSubClass, size_t count, ByteDeserializer& ser)
		{
			const ClassInfo** subClasses = (const ClassInfo**)arrayOfTSubClass;

			for (size_t i = 0; i < count; i++)
			{
				NameHashT hash = 0;
				ser >> hash;
				subClasses[i] = GMetaSys()->FindClass(hash);
			}
		}
	};



	//////////////////////////////////////////////////////////////////////////
	UENGINE_API bool USaveArchive(Object* root, ByteSerializer& ser, bool cooked, bool compress)
	{
		ArchiveHeadData head;
		head.mCompress = compress;
		head.mCooked = cooked;

		ByteSerializer serData;

		if (cooked)
		{
			ObjectSerializerCooked serializerCooked;
			serializerCooked.Save(root, serData);
		}
		else
		{
			ULOG_ERROR("not implemented yet use cooked instead");
			return false;
		}

		if (compress)
		{
			size_t compressedSize = 0;
			if (void* compressedBuffer = UCompressBuffer(serData.GetData(), serData.GetSize(), compressedSize))
			{
				head.mSize = (uint32)compressedSize;
				head.Write(ser);
				ser.Bytes(compressedBuffer, compressedSize);
				MemFree(compressedBuffer);
				return true;
			}
			else
			{
				ULOG_ERROR("failed to compress buffer");
				return false;
			}
		}
		else
		{
			head.mSize = (uint32)serData.GetSize();
			head.Write(ser);
			ser.Bytes(serData.GetData(), serData.GetSize());
			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	UENGINE_API Object* ULoadArchive(ByteDeserializer& ser)
	{
		ArchiveHeadData headData;
		headData.Read(ser);

		if (ser.HasError()) return nullptr;

		//size is zero should I consider it a fault or ...?
		if (headData.mSize == 0) return nullptr;


		size_t memStreamSize = headData.mSize;
		void* memStreamData = MemAlloc(headData.mSize);

		ser.Bytes(memStreamData, memStreamSize);

		if (headData.mCompress)
		{
			size_t decompressedSize = 0;
			if (void* decompressedData = UDeCompressBuffer(memStreamData, memStreamSize, decompressedSize))
			{
				memStreamSize = decompressedSize;
				MemFree(memStreamData);
				memStreamData = decompressedData;
			}
			else
			{
				ULOG_ERROR("failed to decompress buffer");
				MemFreeSafe(memStreamData);
				return nullptr;
			}
		}


		if (headData.mCooked)
		{
			ObjectSerializerCooked archive;

			Object* loadedObj = archive.Load(ByteDeserializer(memStreamData, memStreamSize));
			MemFree(memStreamData);
			return loadedObj;
		}
		else
		{
			ULOG_ERROR("not implemented yet use cooked instead");
			return nullptr;
		}
	}

};