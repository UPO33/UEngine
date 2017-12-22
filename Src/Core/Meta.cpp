#include "Meta.h"
#include "ByteSerializer.h"

namespace UCore
{

	size_t GetPropertyTypeSize(EMetaType type)
	{
		switch (type)
		{
		case EMetaType::EPT_bool: return 1;
		case EMetaType::EPT_int8: return 1;
		case EMetaType::EPT_uint8: return 1;
		case EMetaType::EPT_int16: return 2;
		case EMetaType::EPT_uint16: return 2;
		case EMetaType::EPT_int32: return 4;
		case EMetaType::EPT_uint32: return 4;
		case EMetaType::EPT_int64: return 8;
		case EMetaType::EPT_uint64: return 8;
		case EMetaType::EPT_float: return 4;
		case EMetaType::EPT_double: return 8;

		case EMetaType::EPT_enum: return META_ENUM_SIZE;

		case EMetaType::EPT_TObjectPtr: return sizeof(TObjectPtr<Object>);
		case EMetaType::EPT_TSubClass: return sizeof(TSubClass<Object>);
		case EMetaType::EPT_ObjectPoniter: return sizeof(Object*);
		}

		return 0;
	}

	UCORE_API bool IsArithmetic(EMetaType type)
	{
		switch (type)
		{
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
			return true;
		}
		return false;
	}

	UCORE_API bool IsTemplate(EMetaType type)
	{
		switch (type)
		{
		case EMetaType::EPT_TArray:
		case EMetaType::EPT_TObjectPtr:
		case EMetaType::EPT_TSubClass:
			return true;
		}
		return false;
	}

	UCORE_API MetaSys* GMetaSys()
	{
		static TInstance<MetaSys> Instance;
		return Instance;
	}

	const ClassInfo* MetaSys::FindClass(Name className) const
	{
		return Cast<ClassInfo>(FindType(className));
	}

	const ClassInfo* MetaSys::FindClass(Name::HashT classNameHash) const
	{
		return Cast<ClassInfo>(FindType(classNameHash));
	}

	const EnumInfo* MetaSys::FindEnum(Name enumName) const
	{
		return Cast<EnumInfo>(FindType(enumName));
	}

	const FieldInfo* MetaSys::FindType(Name name) const
	{
		for (FieldInfo* iter : mRegisteredTypes)
		{
			if (iter && iter->GetName() == name)
				return iter;
		}
		return nullptr;
	}

	const FieldInfo* MetaSys::FindType(Name::HashT typeNameHash) const
	{
		for (FieldInfo* iter : mRegisteredTypes)
		{
			if (iter && iter->GetName().GetHash() == typeNameHash)
				return iter;
		}
		return nullptr;
	}

	void MetaSys::GetSubClassesOf(const ClassInfo* theClass, TArray<const ClassInfo*>& out) const
	{
		for (const FieldInfo* iter : mRegisteredTypes)
		{
			if (const ClassInfo* iterClass = Cast<ClassInfo>(iter))
			{
				if (iterClass->GetParentClass() == theClass)
					out.AddUnique(iterClass);
			}
		}
	}

	void MetaSys::RegClass(const ZZ_ClassRegParam& regParam, ClassInfo*& outClassInfo)
	{
		if (const ClassInfo* foundClass = FindClass(Name(regParam.mClassName))) // is already registered?
		{
			ULOG_FATAL("[%] is already registerd", regParam.mClassName);
		}

		ClassInfo* newClass = new ClassInfo;
		newClass->Init(regParam);
		outClassInfo = newClass;

		mRegisteredTypes.Add(TWPtr<ClassInfo>(newClass));
		ULOG_SUCCESS("class [%] registered", newClass->GetName());
	}

	void MetaSys::UnregClass(ClassInfo*& inOutClass)
	{
		if (FindClass(inOutClass->GetName()) == nullptr) //is not registered?
		{
			ULOG_FATAL("[%] has not been registered already", inOutClass->GetName());
		}

		delete inOutClass;
		inOutClass = nullptr;
	}
	inline String UGetUserFriendlyClassName(Name className)
	{
		//#TODO must return user friendly name of a class e.g UCore::Vec3 should return Vec3
		return String();
	}
	inline String UGetUserFriendlyPropertyName(Name propertyName)
	{
		//#TODO must return user friendly name of the property e.g mEnemyID returns Enemy ID
		return String();
	}

	const ClassInfo* ClassInfo::GetParentClass() const
	{
		if (HasParent())
		{
			if (mParentClass.IsValid())
			{
				return mParentClass.Get();
			}
			else
			{
				const ClassInfo* foundClass = GMetaSys()->FindClass(mParentClassName);
				mParentClass = foundClass;
				if (!foundClass)
				{
					//#TODO should we crash?
					ULOG_ERROR("failed to get parent class of [%]. parent class name: [%] ", GetName(), GetParentClassName());
				}
				return foundClass;
			}
		}
		return nullptr;
	}
	//returns the root class, if its root itself returns null
	const ClassInfo* ClassInfo::GetRootClass() const
	{
		const ClassInfo* parent = GetParentClass();
		while (parent)
		{
			auto pp = parent->GetParentClass();
			if (pp == nullptr)
				return parent;

			parent = pp;
		}
		return nullptr;
	}

	bool ClassInfo::HasProperty(const PropertyInfo* propertyPtr, bool checkInheritedPropertiesToo) const
	{
		if (propertyPtr == nullptr) return false;

		if (mProperties.HasElement(propertyPtr))
			return true;

		if (checkInheritedPropertiesToo)
		{
			if (const ClassInfo* parent = GetParentClass())
				return parent->HasProperty(propertyPtr, true);
		}
		return false;
	}

	const PropertyInfo* ClassInfo::FindProperty(Name propertyName, bool checkInheritedPropertiesToo) const
	{
		for (const PropertyInfo* prp : mProperties)
		{
			if (prp && prp->GetName() == propertyName)
				return prp;
		}

		if (checkInheritedPropertiesToo)
		{
			if (const ClassInfo* parent = GetParentClass())
				return parent->FindProperty(propertyName, true);
		}
		return nullptr;
	}

	const PropertyInfo* ClassInfo::FindProperty(Name::HashT propertyNameHash, bool checkInheritedPropertiesToo) const
	{
		for (const PropertyInfo* prp : mProperties)
		{
			if (prp && prp->GetName().GetHash() == propertyNameHash)
				return prp;
		}

		if (checkInheritedPropertiesToo)
		{
			if (const ClassInfo* parent = GetParentClass())
				return parent->FindProperty(propertyNameHash, true);
		}
		return nullptr;
	}

	const TArray<const ClassInfo*>& ClassInfo::GetClassChain() const
	{
		if (mClassChain.Length() == 0)
		{
			mClassChain.Add(this);

			const ClassInfo* parent = GetParentClass();
			while (parent)
			{
				mClassChain.Add(parent);
				parent = parent->GetParentClass();
			}

			mClassChain.Reverse();
		}

		return mClassChain;
	}


	const TArray<const ClassInfo*>& ClassInfo::GetSubClasses() const
	{
		if (mSubClasses.Length() == 0) //bot baked yet?
		{
			GMetaSys()->GetSubClassesOf(this, mSubClasses);
		}

		return mSubClasses;
	}

	bool ClassInfo::IsBaseOf(const ClassInfo* base) const
	{
		UASSERT(base);

		if (this == base) return true;

		const ClassInfo* iter = GetParentClass();

		while (iter)
		{
			if (iter == base) return true;
			iter = iter->GetParentClass();
		}
		return false;
	}

	void ClassInfo::CallDefaultConstructor(void* object) const
	{
		UASSERT(object && mCTor);
		mCTor(object);
	}

	void ClassInfo::CallDestructor(void* object) const
	{
		UASSERT(object && mDTor);
		mDTor(object);
	}

	void ClassInfo::CallCopyConstructor(void* object, const void* copyfrom) const
	{
		UASSERT(object && copyfrom && mCCTor);
		mCCTor(object, copyfrom);
	}

	void ClassInfo::CallMetaSerialize(void* object, ByteSerializer& ser) const
	{
		UASSERT(object && mMetaSerialize);
		mMetaSerialize(object, ser);
	}
	void ClassInfo::CallMetaDeserialize(void* object, ByteDeserializer& ser) const
	{
		UASSERT(object && mMetaDeserialize);
		mMetaDeserialize(object, ser);
	}

	ClassInfo::~ClassInfo()
	{
		for (const PropertyInfo*& prp : mProperties)
		{
			SafeDelete(prp);
		}
		mProperties.Empty();
	}

	void ClassInfo::Init(const ZZ_ClassRegParam& classRegParam)
	{
		mAttributes = classRegParam.mAttributes;
		mHeaderFileLine = classRegParam.mHeaderFileLine;
		mHeaderFileName = classRegParam.mHeaderFileName;
		mSourceFileLine = classRegParam.mSourceFileLine;
		mSourceFileName = classRegParam.mSourceFileName;
		mName = classRegParam.mClassName;
		mNameUserFriendly = UGetUserFriendlyClassName(classRegParam.mClassName);
		mSize = classRegParam.mClassSize;
		mAlign = classRegParam.mClassAlign;

		mCTor = classRegParam.mDefaulConstructor;
		mCCTor = classRegParam.mCopyConstructor;
		mDTor = classRegParam.mDestructor;

		mMetaSerialize = classRegParam.mMetaSerialize;
		mMetaDeserialize = classRegParam.mMetaDeserialize;

		mParentClassName = classRegParam.mParentClassName;


		//registering properties
		{

			for (int iProperty = 0; iProperty < classRegParam.mNumProperty; iProperty++)
			{
				const ZZ_PropertyRegParam& prpRegParam = classRegParam.mProperties[iProperty];
				Name prpRegParamName = prpRegParam.mName;

				bool propertyAlreadyRegistered = mProperties.HasElementLambda([prpRegParamName](const PropertyInfo* prp) {
					return (prp && prp->GetName() == prpRegParamName);
				});

				if (propertyAlreadyRegistered)
				{
					//#TODO should we crash?
					ULOG_ERROR("property [%::%] already registered", classRegParam.mClassName, prpRegParam.mName);
				}
				else
				{
					PropertyInfo* newProperty = new PropertyInfo;
					newProperty->Init(this, prpRegParam);
					mProperties.Add(newProperty);
				}
			}
		}
	}





	void* PropertyInfo::Map(void* instance) const
	{
		return (void*)(((size_t)instance) + mByteOffset);
	}



	void PropertyInfo::Init(ClassInfo* owner, const ZZ_PropertyRegParam& regParam)
	{
		mOwner = owner;
		mName = regParam.mName;
		mNameUserFriendly = UGetUserFriendlyPropertyName(regParam.mName);
		mByteOffset = regParam.mByteOffset;
		mAttributes = regParam.mAttributes;

		mSelf = TriTypeData(regParam.mPropertyType, regParam.mPropertyTypeName);
		mArg0 = TriTypeData(regParam.mArg0Type, regParam.mArg0TypeName);
		mAr0Arg0 = TriTypeData(regParam.mArg0Arg0Type, regParam.mArg0Arg0TypeName);

		mSize = regParam.mSize;
		mAlign = regParam.mAlign;
	}

	void EnumInfo::Init(const ZZ_EnumRegParam& regParam)
	{

	}

	const FieldInfo* TriTypeData::GetPtr() const
	{
		if (mType == EMetaType::EPT_ObjectPoniter || mType == EMetaType::EPT_Class || mType == EMetaType::EPT_enum)
		{
			if (mTypePtr.IsValid())
			{
				return mTypePtr.Get();
			}
			else
			{
				const FieldInfo* found = GMetaSys()->FindType(mTypeName);
				mTypePtr = found;
				if (!found)
				{
					ULOG_ERROR("failed to get the type of %", mTypeName);
				}
				return found;
			}
		}
		return nullptr;
	}



	void TriTypeData::MetaSerialize(ByteSerializer& ser)
	{
		uint8 iType = (uint8)mType;
		ser << iType;
		mTypeName.MetaSerialize(ser);
	}

	void TriTypeData::MetaDeserialize(ByteDeserializer& dser)
	{
		uint8 iType = 0;
		dser >> iType;
		mTypeName.MetaDeserialize(dser);
	}

}
