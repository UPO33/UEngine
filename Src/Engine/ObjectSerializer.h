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
	struct ObjectSerializer
	{
		TArray<Object*> mObjects;

		static void WClassName(const ClassInfo* pClass, ByteSerializer& ser)
		{
			if (pClass)
				pClass->GetName().MetaSerialize(ser);
			else
				Name().MetaSerialize(ser);
		}
		static const ClassInfo* RClassName(ByteDeserializer& dser)
		{
			Name cn;
			cn.MetaDeserialize(dser);
			return GMetaSys()->FindClass(cn);
		}

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

				ULOG_MESSAGE("% object collected", mObjects.Length());
			}

			//estimating the capacity required for serializer
			{
				size_t estimate = 0;
				for (Object* pObj : mObjects)
				{
					estimate += pObj->GetClass()->GetSize();
				}
				ser.IncCapacity(estimate * 8);
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

		//////////////////////////////////////////////////////////////////////////
		void WObjects(ByteSerializer& ser)
		{
			uint32 numObject = (uint32)mObjects.Length();
			ser << numObject;

			//writing class name of objects
			for (uint32 iObj = 0; iObj < numObject; iObj++)
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
				ser.PushSizeToken();
				WObject(mObjects[iObj], ser);
				ser.PopSizeToken();
			}
		}

		/*
		uint32 numObject, className[numObject], uint32 parentIndex[numObject]
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
				if (objectClass && objectClass->IsBaseOf<Object>())
				{
					Object* newObject = ObjectUtility::NewDefault(objectClass, parent, false);

					mObjects[iObj] = newObject;

					RObject(newObject, ser.ReadToken());
				}
				else
				{
					ser.IgnoreToken();
				}
			}
		}


		void WObject(const Object* pObject, ByteSerializer& ser)
		{
			ser.PushSizeToken();
			((Object*)pObject)->OnMetaSerialize(ser);
			ser.PopSizeToken();

			TArray<PropertyInfo*> propertiesToWrite;

			for (const ClassInfo* pClassIter : pObject->GetClass()->GetClassChain())
			{
				if (pClassIter->GetAttribute<AttrVolatile>())
					continue;

				for (const PropertyInfo* pProperty : pClassIter->GetProperties())
				{
					if (pProperty->GetAttribute<AttrVolatile>() == nullptr)	//is not volatile?
						propertiesToWrite.Add(pProperty);
				}
			}

			uint32 numProperties = propertiesToWrite.Length();
			ser << numProperties;

			for (const PropertyInfo* pProperty : propertiesToWrite)
			{
				WProperty(pProperty, pProperty->Map(pObject), ser);
			}

		}
		//////////////////////////////////////////////////////////////////////////
		void RObject(Object* pObject, ByteDeserializer& ser)
		{
			ByteDeserializer dserFunc = ser.ReadToken();
			pObject->OnMetaDeserialize(dserFunc);
			if (dserFunc.HasError())
				return;

			const ClassInfo* pClass = pObject->GetClass();

			uint32 numProperties = 0;
			ser >> numProperties;

			for (uint32 iProperty; iProperty < numProperties; iProperty++)
			{
				RProperty(pObject, ser);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//[uint32 num properties] [properties]
		void WClass(const ClassInfo* pClass, const void* pInstances, size_t count, ByteSerializer& ser)
		{
			for (size_t iInstance = 0; iInstance < count; iInstance++)
			{
				void* instance = reinterpret_cast<void*>(((size_t)pInstances) + pClass->GetSize() * iInstance);

				//get the top most MetaSerialize function and call it if any
				//#Note: overridden MetaSerialize() must call its parent's MetaSerialize() as well 
				//#Note MetaSerialize() and MetaDeserialize() themselves are in charge of versioning, the number of bytes loeader reads must be equal with the written data
				{
					const ClassInfo* pClassMetaSerialize = nullptr;
					for (const ClassInfo* pClassIter : pClass->GetClassChain())
					{
						if (pClassIter->HasMetaSerialize())
							pClassMetaSerialize = pClassIter;
					}

					ser.PushSizeToken();
					if (pClassMetaSerialize)
						pClassMetaSerialize->CallMetaSerialize(instance, ser);
					ser.PopSizeToken();
				}

				TArray<const PropertyInfo*> propertiesToWrite;

				//collecting properties
				for (const ClassInfo* pClassIter : pClass->GetClassChain())
				{
					if (pClassIter->GetAttribute<AttrVolatile>())
						continue;

					for (const PropertyInfo* pProperty : pClassIter->GetProperties())
					{
						if (pProperty->GetAttribute<AttrVolatile>() == nullptr)	//is not volatile?
							propertiesToWrite.Add(pProperty);
					}
				}
				//write number of properties
				uint32 numProperties = (uint32)propertiesToWrite.Length();
				ser << numProperties;

				//writing properties
				ser.PushSizeToken();
				for (const PropertyInfo* pProperty : propertiesToWrite)
				{
					WProperty(pProperty, pProperty->Map(instance), ser);
				}
				ser.PopSizeToken();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void RClass(const ClassInfo* pDstClass, void* pDstInstances, size_t count, ByteDeserializer& ser)
		{
			for (size_t iInstance = 0; iInstance < count; iInstance++)
			{
				void* dstInstance = reinterpret_cast<void*>(((size_t)pDstInstances) + pDstClass->GetSize() * iInstance);

				//get the top most MetaDeserialize function and call it if any
				{
					const ClassInfo* pClassMetaDeserialize = nullptr;
					for (const ClassInfo* pClassIter : pDstClass->GetClassChain())
					{
						if (pClassIter->HasMetaDeserialize())
							pClassMetaDeserialize = pClassIter;
					}
					ByteDeserializer deserMetaDeserialize = ser.ReadToken();
					if (pClassMetaDeserialize)
						pClassMetaDeserialize->CallMetaDeserialize(dstInstance, deserMetaDeserialize);
				}

				//reading  properties count
				uint32 numProperties = 0;
				ser >> numProperties;
				//reading properties
				ByteDeserializer deserProperties = ser.ReadToken();
				for (uint32 iProperty = 0; iProperty < numProperties; iProperty++)
				{
					RProperty(dstInstance, pDstClass, deserProperties);
				}

			}
		}
		void IgnoreClass(uint32 count, ByteDeserializer& ser)
		{
			for (size_t iInstance = 0; iInstance < count; iInstance++)
			{
				ser.IgnoreToken(); //for OnMetaDeserialize

				//ignore properties count
				uint32 numProperties = 0;
				ser >> numProperties;
				//ignore properties
				ser.IgnoreToken();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//@pProperty		the intended property
		//@pMappedVlaue		pointer to the value of property, for instance if property is int it means int*
		//@stream			the stream to write data to
		//////////////////////////////////////////////////////////////////////////
		//writes: property name, property type, array length if is array
		void WProperty(const PropertyInfo* pProperty, const void* pMappedValue, ByteSerializer& ser)
		{
			//write property name
			pProperty->GetName().MetaSerialize(ser);

			//write property type
			{
				pProperty->GetSelf().MetaSerialize(ser);
				pProperty->GetArg0().MetaSerialize(ser);
				pProperty->GetArg0Arg0().MetaSerialize(ser);
			}

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
					objectPopertyClass = dynamic_cast<const ClassInfo*>(pProperty->GetArg0Arg0().GetPtr());
				else //property is TObjectPtr<Class>
					objectPopertyClass = dynamic_cast<const ClassInfo*>(pProperty->GetArg0().GetPtr());

				WTObjectPtrOrObjectPointer(objectPopertyClass, pValues, count, ser, true);
				break;
			}
			case EMetaType::EPT_ObjectPoniter:
			{
				const ClassInfo* objectPopertyClass = nullptr;
				if (isTArrayProperty) //property is TArray<Object*>
					objectPopertyClass = dynamic_cast<const ClassInfo*>(pProperty->GetArg0().GetPtr());
				else //property is Object*
					objectPopertyClass = dynamic_cast<const ClassInfo*>(pProperty->GetSelf().GetPtr());

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
// 		void RProperty(Object* pObject, ByteDeserializer& ser)
// 		{
// 			//read property name
// 			Name propertyName;
// 			propertyName.MetaDeserialize(ser);
// 			//read property type
// 			TriTypeData propertTypeSelf, propertyTypeArg0, propertyTypeArg0Arg0;
// 			propertTypeSelf.MetaDeserialize(ser);
// 			propertyTypeArg0.MetaDeserialize(ser);
// 			propertyTypeArg0Arg0.MetaDeserialize(ser);
// 
// 			uint32 arrayLength = 0;
// 			if (propertTypeSelf.GetType() == EMetaType::EPT_TArray)
// 			{
// 				ser >> arrayLength;
// 
// 				if (arrayLength == 0)
// 					return;
// 			}
// 
// 			//find property
// 			if (const PropertyInfo* pPropertyFound = pObject->GetClass()->FindProperty(propertyName.GetHash(), true))
// 			{
// 				if (pPropertyFound->GetSelf().GetType() == propertTypeSelf.GetType()) //same type?
// 				{
// 					EMetaType dstMetaType = pPropertyFound->GetSelf().GetType();
// 					void* pDstValue = pPropertyFound->Map(pObject);
// 					size_t dstCount = 1;
// 
// 					if (pPropertyFound->GetSelf().GetType() == EMetaType::EPT_TArray) //found property is TArray?
// 					{
// 						MetaArrayView arrayView(pPropertyFound->Map(pObject), pPropertyFound->GetArg0());
// 						arrayView.AddDefault(arrayLength);
// 
// 						pDstValue = arrayView.Elements();
// 						dstMetaType = pPropertyFound->GetArg0().GetType();
// 						dstCount = arrayLength;
// 
// 						if (pPropertyFound->GetArg0().GetType() == propertyTypeArg0.GetType()) //array element is same?
// 						{
// 							for (uint32 i = 0; i < dstCount; i++)
// 							{
// 								void* pArrayElement = arrayView.GetElementAt(i);
// 
// 							}
// 						}
// 					}
// 
// 
// 					switch (dstMetaType)
// 					{
// 					case EMetaType::EPT_Unknown:
// 						break;
// 					case EMetaType::EPT_bool:
// 					case EMetaType::EPT_int8:
// 					case EMetaType::EPT_uint8:
// 					case EMetaType::EPT_int16:
// 					case EMetaType::EPT_uint16:
// 					case EMetaType::EPT_int32:
// 					case EMetaType::EPT_uint32:
// 					case EMetaType::EPT_int64:
// 					case EMetaType::EPT_uint64:
// 					case EMetaType::EPT_float:
// 					case EMetaType::EPT_double:
// 					case EMetaType::EPT_enum:
// 						ser.Bytes(pDstValue, GetPropertyTypeSize(dstMetaType) * dstCount);
// 						break;
// 
// 					case EMetaType::EPT_TSubClass:
// 						RTSubClass(pDstValue, dstCount, ser);
// 						break;
// 
// 					case EMetaType::EPT_ObjectPoniter:
// 					{
// 						break;
// 					}
// 					case EMetaType::EPT_TObjectPtr
// 					{
// 						RTObjectPtrOrObjectPointer(pDstValue, dstMetaType)
// 						break;
// 					}
// 
// 					case EMetaType::EPT_Class:
// 					{
// 						const ClassInfo* pDstClass = dynamic_cast<const ClassInfo*>(nullptr);
// 						const ClassInfo* pSrcClass = dynamic_cast<const ClassInfo*>(nullptr);
// 
// 						RClass(pDstClass, pSrcClass, pDstValue, dstCount, ser);
// 					}
// 
// 					}
// 
// 				}
// 				else
// 				{
// 					ULOG_WARN("reading property [%] failed. the object has not such a property", propertyName);
// 				}
// 			}
// 		}

		//////////////////////////////////////////////////////////////////////////
		void RProperty(void* pInstance, const ClassInfo* pClass, ByteDeserializer& ser)
		{
			//read property name
			Name propertyName;
			propertyName.MetaDeserialize(ser);
			//read property type
			TriTypeData propertTypeSelf, propertyTypeArg0, propertyTypeArg0Arg0;
			propertTypeSelf.MetaDeserialize(ser);
			propertyTypeArg0.MetaDeserialize(ser);
			propertyTypeArg0Arg0.MetaDeserialize(ser);

			uint32 arrayLength = 0;
			if (propertTypeSelf.GetType() == EMetaType::EPT_TArray)
			{
				ser >> arrayLength;

				if (arrayLength == 0)
					return;
			}

			//find property
			if (const PropertyInfo* pPropertyFound = pClass->FindProperty(propertyName.GetHash(), true))
			{
				if (pPropertyFound->GetSelf().GetType() == propertTypeSelf.GetType()) //same type?
				{
					EMetaType dstMetaType = pPropertyFound->GetSelf().GetType();
					TriTypeData dstType = pPropertyFound->GetSelf();
					TriTypeData srcType = propertTypeSelf;
					void* pDstValue = pPropertyFound->Map(pInstance);
					size_t dstCount = 1;

					if (pPropertyFound->GetSelf().GetType() == EMetaType::EPT_TArray) //found property is TArray?
					{
						if (pPropertyFound->GetArg0().GetType() != propertyTypeArg0.GetType())
						{
							//IgnoreBytes
						}
						else
						{
							MetaArrayView arrayView(pPropertyFound->Map(pInstance), pPropertyFound->GetArg0());
							arrayView.AddDefault(arrayLength);

							pDstValue = arrayView.Elements();
							dstType = pPropertyFound->GetArg0();
							srcType = propertyTypeArg0;
							dstMetaType = pPropertyFound->GetArg0().GetType();
							dstCount = arrayLength;
						}
					}


					switch (dstMetaType)
					{
					case EMetaType::EPT_Unknown:
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
						ser.Bytes(pDstValue, GetPropertyTypeSize(dstMetaType) * dstCount);
						break;

					case EMetaType::EPT_TSubClass:
						RTSubClass(pDstValue, dstCount, ser);
						break;

					case EMetaType::EPT_Class:
					{
						//the class of property we are writing to
						const ClassInfo* pDstClass = dynamic_cast<const ClassInfo*>(dstType.GetPtr());

						RClass(pDstClass, pDstValue, dstCount, ser);
					}

					case EMetaType::EPT_TObjectPtr:
						RTObjectPtrOrObjectPointer(dynamic_cast<const ClassInfo*>(dstType.GetPtr()), true, pDstValue, dstCount, ser);
						break;

					case EMetaType::EPT_ObjectPoniter:
						RTObjectPtrOrObjectPointer(dynamic_cast<const ClassInfo*>(dstType.GetPtr()), false, pDstValue, dstCount, ser);
						break;
					}
				}
				else //ignore bytes
				{
					EMetaType ignoreType = 0;
					uint32 ignoreCount = 1;

					switch (ignoreType)
					{
					case EMetaType::EPT_Unknown:
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
						ser.IgnoreBytes(GetPropertyTypeSize(ignoreType) * ignoreCount);
						break;

					case EMetaType::EPT_TSubClass:
						IgnoreTSubClass(ignoreCount, ser);
						break;

					case EMetaType::EPT_Class:
						IgnoreClass(ignoreCount, ser);
						break;

					case EMetaType::EPT_TObjectPtr:
					case EMetaType::EPT_ObjectPoniter:
						IgnoreTObjectPtrOrObjectPointer(ignoreCount, ser);
						break;
					}
					ULOG_WARN("reading property [%] failed. the object has not such a property", propertyName);
				}
			}
		}

		

		//////////////////////////////////////////////////////////////////////////
		//[bool isAsset][class name of object][AssetID or index of object]
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

				bool isAsset = pObjectClass->IsBaseOf<Asset>();
				ser << isAsset;
				WClassName(pObject ? pObject->GetClass() : nullptr, ser);

				if (isAsset) //is asset?
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
		//////////////////////////////////////////////////////////////////////////
		void RTObjectPtrOrObjectPointer(const ClassInfo* pDstPropertyClass, bool bDstIsTObjectPtr, void* pDsts, size_t count, ByteDeserializer& dser)
		{
			UASSERT(pDstPropertyClass->IsObjectClass());

			//casting as array of Object* and TObjectPtr
			auto asTObjectPtr = (TObjectPtr<Object>*)(pDsts);
			auto asObjectStar = (Object**)(pDsts);

			for (size_t i = 0; i < count; i++)
			{
				Object* pObject = nullptr;

				if (bDstIsTObjectPtr)
					pObject = asTObjectPtr[i].Get();
				else
					pObject = asObjectStar[i];

				bool isAsset = false;
				dser >> isAsset;
				const ClassInfo* writtenObjectClass = RClassName(dser);
				bool classIsValid = writtenObjectClass && writtenObjectClass->IsBaseOf(pDstPropertyClass);

				if (isAsset)
				{
					AssetID assetID;
					assetID.MetaDeserialize(dser);
					
					if(classIsValid)
					{
						if (bDstIsTObjectPtr)
							mPointerPhaseAssetPtrProperties.Add(SAssetProperty((TObjectPtr<Asset>*)(asObjectStar + i), bDstIsTObjectPtr, assetID));
						else
							mPointerPhaseAssetPtrProperties.Add(SAssetProperty((Asset**)(asObjectStar + i), bDstIsTObjectPtr, assetID));
					}
				}
				else
				{
					uint32 index = ~uint32(0);
					dser >> index;

					if(classIsValid)
					{
						//even invalid index is added to array, pointer phase must zero it
						if (bDstIsTObjectPtr)
							mPointerPhaseObjectPtrProperties.Add(SObjectProperty(asTObjectPtr + i, true, index));
						else
							mPointerPhaseObjectPtrProperties.Add(SObjectProperty(asObjectStar + i, false, index));
					}

				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void IgnoreTObjectPtrOrObjectPointer(size_t count, ByteDeserializer& dser)
		{
			for (size_t i = 0; i < count; i++)
			{
				bool isAsset = false;
				dser >> isAsset;
				const ClassInfo* writtenObjectClass = RClassName(dser);

				if (isAsset)
				{
					AssetID assetID;
					assetID.MetaDeserialize(dser);
				}
				else
				{
					uint32 index = ~uint32(0);
					dser >> index;
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

				if (iter.mIndex != ~uint32(0))
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
		static void WTSubClass(const void* arrayOfTSubClass, size_t count, ByteSerializer& ser)
		{
			const ClassInfo** subClasses = (const ClassInfo**)arrayOfTSubClass;

			for (size_t i = 0; i < count; i++)
			{
				WClassName(subClasses[i], ser);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static void RTSubClass(void* arrayOfTSubClass, size_t count, ByteDeserializer& ser)
		{
			const ClassInfo** subClasses = (const ClassInfo**)arrayOfTSubClass;

			for (size_t i = 0; i < count; i++)
			{
				subClasses[i] = RClassName(ser);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static void IgnoreTSubClass(size_t count, ByteDeserializer& ser)
		{
			for (size_t i = 0; i < count; i++)
				RClassName(ser);
		}
	};
};