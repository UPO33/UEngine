#pragma once

#include "Array.h"
#include "String.h"
#include "Name.h"
#include "ObjectPtr.h"
#include "Object.h"

#ifdef UCOMPILER_GCC
#include <cxxabi.h>
#endif

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class ClassInfo;
	struct ZZ_ClassRegParam;
	class PropertyInfo;
	struct ZZ_PropertyRegParam;
	class MetaSys;
	class EnumInfo;
	struct ZZ_EnumRegParam;


	//the maximum size that an attribute class can be
	static const unsigned MAX_ATTRIBUTE_SIZE = 128;
	//maximum  number of attributes that a class, property , etc can hold
	static const unsigned MAX_ATTRIBUTE = 16;
	//the only supported size for meta enum
	static const unsigned META_ENUM_SIZE = sizeof(uint32);

	////base class for attributes, inherit from this to implement your own attribute
	struct AttrBase
	{
		virtual ~AttrBase() {}
	};
	////lets editor clamp the value of property. int, float, vector ...
	struct AttrMinMax : public AttrBase
	{
		float mMin;
		float mMax;

		AttrMinMax(float min, float max) : mMin(min), mMax(max) {}
	};
	////adds comment to class, function, whatever, ..
	struct AttrComment : public AttrBase
	{
		const char* mComment;

		AttrComment(const char* comment) : mComment(comment) {}
	};
	/////overrides the displaying name of a class, property, ...
	struct AttrDisplayName : public AttrBase
	{
		const char* mName;

		AttrDisplayName(const char* name) : mName(name) {}
	};
	////specify a category for class, property, function ...
	struct AttrCategory : public AttrBase
	{
		const char* mCategory;

		AttrCategory(const char* category) : mCategory(category) {}
	};
	////property can't be edited in editor, by default properties are editable
	struct AttrUneditable : public AttrBase {};
	////property is hidden in editor
	struct AttrHidden : public AttrBase {};
	////a volatile property or class will not be serialized/deserialized
	struct AttrVolatile : public AttrBase {};





	//////////////////////////////////////////////////////////////////////////
	struct AttributePack
	{
		struct Buffer
		{
			char mBuffer[MAX_ATTRIBUTE_SIZE];
		} mAttributes[MAX_ATTRIBUTE];

		unsigned		mNumAttributes = 0;

		template<typename TAttribute> const TAttribute* GetAttribute() const
		{
			for (unsigned iAttrib = 0; iAttrib < mNumAttributes; iAttrib++)
				if (TAttribute* casted = dynamic_cast<TAttribute*>((AttrBase*)(mAttributes[iAttrib].mBuffer)))
					return casted;

			return nullptr;
		}
	};

	inline void ZZPackAttribute(AttributePack* out)
	{}
	template <typename T, typename... TArgs> void ZZPackAttribute(AttributePack* out, T first, TArgs... args)
	{
		new (out->mAttributes + out->mNumAttributes) T(first);
		out->mNumAttributes++;
		ZZPackAttribute(out, args...);
	}

	//////////////////////////////////////////////////////////////////////////
	enum EMetaType
	{
		EPT_Unknown,
		EPT_bool,
		EPT_int8, EPT_uint8, EPT_int16, EPT_uint16, EPT_int32, EPT_uint32, EPT_int64, EPT_uint64,	//integral types
		EPT_float, EPT_double,	//floating point types
		EPT_enum,
		EPT_TArray, EPT_TStackArray, EPT_TObjectPtr, EPT_TSubClass,	//templates

		EPT_ObjectPoniter, //Object*
		EPT_Class
	};

	//return the size of property, zero if type is Unknown or Class
	UCORE_API size_t GetPropertyTypeSize(EMetaType type);

	//////////////////////////////////////////////////////////////////////////
	class UCORE_API FieldInfo : public ISmartBase
	{
	public:
		friend MetaSys;

		Name			GetName() const { return mName; }
		const String&	GetNameUserFriendly() const { return mNameUserFriendly; }
		size_t			GetSize() const { return mSize; }
		size_t			GetAlign() const { return mAlign; }

		template<typename TAttribute> const TAttribute* GetAttribute() const
		{
			return mAttributes.GetAttribute<TAttribute>();
		}
		const char* GetComment() const
		{
			if (const AttrComment* comment = GetAttribute<AttrComment>())
				return comment->mComment;
			return nullptr;
		}
		const char* GetCategory() const
		{
			if (const AttrCategory* category = GetAttribute<AttrCategory>())
				return category->mCategory;
			return nullptr;
		}
		template<class T> T* Cast() { return dynamic_cast<T*>(this); }
		template<class T> const T* Cast() const { return dynamic_cast<const T*>(this); }

	protected:
		AttributePack		mAttributes;
		Name				mName;
		String				mNameUserFriendly;
		size_t				mSize;
		size_t				mAlign;
	};

	template<typename T> T* Cast(FieldInfo* in)
	{
		return dynamic_cast<T*>(in);
	}
	template<typename T> const T* Cast(const FieldInfo* in)
	{
		return dynamic_cast<const T*>(in);
	}
	template<typename T> T* CastSure(FieldInfo* in)
	{
		T* result = dynamic_cast<T*>(in);
		UFATAL(result);
		return result;
	}
	template<typename T> const T* CastSure(const FieldInfo* in)
	{
		const T* result = dynamic_cast<const T*>(in);
		UFATAL(result);
		return result;
	}

	//return true if 'type' is arithmetic or enum
	UCORE_API bool IsArithmetic(EMetaType type);
	UCORE_API bool IsTemplate(EMetaType type);

	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API TriTypeData
	{
		TriTypeData(EMetaType theType = EMetaType::EPT_Unknown, Name name = Name())
			: mType(theType), mTypeName(name) {}

		EMetaType			GetType() const { return mType; }
		Name				GetTypeName() const { return mTypeName; }
		const FieldInfo*	GetPtr() const;

	private:
		EMetaType					mType;
		Name						mTypeName;
		mutable TWPtr<FieldInfo>	mTypePtr;


	};

	//////////////////////////////////////////////////////////////////////////
	//keeps the information about a property of a class
	class UCORE_API PropertyInfo : public FieldInfo
	{
	public:
		friend MetaSys;
		friend ClassInfo;

		const TriTypeData&		GetSelf() const { return mSelf; }
		const TriTypeData&		GetArg0() const { return mArg0; }
		const TriTypeData&		GetArg0Arg0() const { return mAr0Arg0; }
		//returns the offset of this member variable from root class in bytes
		size_t					GetByteOffset() const { return mByteOffset; }
		const ClassInfo*		GetOwner() const { return mOwner; }
		void*					Map(void* instance) const;
		const void*				Map(const void* instance) const { return Map((void*)instance); }

		Name					GetTypeName() const { return GetSelf().GetTypeName(); }
		EMetaType				GetType() const { return GetSelf().GetType(); }
		const FieldInfo*		GetTypePtr() const { return GetSelf().GetPtr(); }

	private:
		void Init(ClassInfo* owner, const ZZ_PropertyRegParam& regParam);

		ClassInfo*			mOwner;
		size_t				mByteOffset;

		TriTypeData		mSelf;
		TriTypeData		mArg0;
		TriTypeData		mAr0Arg0;
	};

	using FPMetaClassDefaultConstructor = void(*)(void* instance);
	using FPMetaClassCopyConstructor = void(*)(void* instance, const void* copy);
	using FPMetaClassDestructor = void(*)(void* instance);

	using FPMetaSerialize = TMFP<void(ByteSerializer&)>;
	using FPMetaDeserialize = TMFP<void(ByteDeserializer&)>;

	//holds the information of a class for reflection, serialization , ...
	class UCORE_API ClassInfo : public FieldInfo
	{
	public:
		friend MetaSys;
		friend PropertyInfo;

		const ClassInfo*					GetParentClass() const;
		const ClassInfo*					GetRootClass() const;
		bool								HasParent() const { return !mParentClassName.IsEmpty(); }
		Name								GetParentClassName() const { return mParentClassName; }
		const TArray<const PropertyInfo*>&	GetProperties() const { return mProperties; }
		size_t								GetNumProperty() const { return mProperties.Length(); }
		const PropertyInfo*					GetProperty(size_t index) const { return mProperties[index]; }
		bool								HasProperty(const PropertyInfo* propertyPtr, bool checkInheritedPropertiesToo) const;
		const PropertyInfo*					FindProperty(Name propertyName, bool checkInheritedPropertiesToo) const;
		const PropertyInfo*					FindProperty(Name::HashT propertyNameHash, bool checkInheritedPropertiesToo) const;
		const TArray<const ClassInfo*>&		GetClassChain() const;
		const TArray<const ClassInfo*>&		GetSubClasses() const;
		bool								IsBaseOf(const ClassInfo* base) const;

		//a default constructible class has default constructor and destructor and can be instantiated
		bool								IsDefaultConstructable() const { return mCTor; }
		bool								HasDefaultConstructor() const { return mCTor; }
		bool								HasDestructor() const { return mDTor; }
		bool								HasCopyConstructor() const { return mCCTor; }
		bool								HasMetaSerialize() const { return mMetaSerialize; }
		bool								HasMetaDeserialize() const { return mMetaDeserialize; }

		void								CallDefaultConstructor(void* object) const;
		void								CallDestructor(void* object) const;
		void								CallCopyConstructor(void* object, const void* copyfrom) const;
		void								CallMetaSerialize(void* object, ByteSerializer& ser) const;
		void								CallMetaDeserialize(void* object, ByteDeserializer& ser) const;

		bool								IsObjectClass() const { return IsBaseOf(Object::GetClassStatic()); }

		template<typename T> bool IsBaseOf() const
		{
			return IsBaseOf(T::GetClassStatic());
		}

		ClassInfo(){}
		~ClassInfo();
	private:

		void Init(const ZZ_ClassRegParam& regParam);

		Name							mParentClassName;
		mutable TWPtr<ClassInfo>		mParentClass;

		TArray<const PropertyInfo*>			mProperties;
		mutable TArray<const ClassInfo*>	mClassChain;
		mutable TArray<const ClassInfo*>	mSubClasses;

		FPMetaClassDefaultConstructor	mCTor;
		FPMetaClassCopyConstructor		mCCTor;
		FPMetaClassDestructor			mDTor;

		FPMetaSerialize					mMetaSerialize;
		FPMetaDeserialize				mMetaDeserialize;

		const char*			mHeaderFileName;
		unsigned			mHeaderFileLine;
		const char*			mSourceFileName;
		unsigned			mSourceFileLine;

		bool		mErrorParentNotFound;
	};

	//////////////////////////////////////////////////////////////////////////
	template<class TClass> class TSubClass
	{
	public:
		static_assert(TT_IsMetaClass<TClass>::value, "TClass must be meta class");

		typedef TClass ClassType;

		TSubClass() : mClass(nullptr) {}
		TSubClass(std::nullptr_t) : mClass(nullptr) {}
		TSubClass(const ClassInfo* classPtr)
		{
			if (classPtr && classPtr->IsBaseOf<TClass>())
				mClass = classPtr;
			else
				mClass = nullptr;
		}
		TSubClass& operator = (const ClassInfo* classPtr)
		{
			new (this) TSubClass(classPtr);
			return *this;
		}
		template<class T> void Set()
		{
			static_assert(std::is_base_of<TClass, T>::value, "T must be derived from TClass");

			const ClassInfo* theClass = T::GetClassStatic();
			mClass = theClass;
		}
		const ClassInfo* Get() const { return mClass; }
		operator const ClassInfo* () const { return mClass; }
		const ClassInfo* operator -> () const { return (const ClassInfo*)mClass; }

	private:
		const ClassInfo* mClass;
	};


	class EnumElementInfo
	{
	public:
		friend EnumInfo;

		unsigned		GetValue() const { return mValue; }
		const String&	GetName() const { return mName; }
		const String&	GetNameUserFriendly() const { return mNameUserFriendly; }

	private:
		String		mName;
		String		mNameUserFriendly;
		unsigned	mValue;
	};


	struct ZZ_EnumRegParam
	{
		const char*		mEnumName;
		const char*		mComment;
		bool			mIsBitMask;
		unsigned		mNumElement;
		unsigned*		mElementsValue;
		const char*		mElementsName; 	//#__VA_ARGS__
		const char*		mSourceFileName;
		unsigned		mLineNumber;
	};


	class EnumInfo : public FieldInfo
	{
	public:

		const TArray<EnumElementInfo>&	GetElements() const { return mElements; }
		const String&					GetComment() const { return mComment; }
		bool							IsBitMask() const { return mIsBitMask; }

	private:
		void Init(const ZZ_EnumRegParam& regParam);

		TArray<EnumElementInfo>			mElements;
		String							mComment;
		bool							mIsBitMask;
	};

	//get the name of TClass using typeid, sample: "UCore::Vec3"
	template<class TClass> inline Name ZZGetClassName()
	{
#ifdef UCOMPILER_MSVC
		return Name(strchr(typeid(TClass).name(), ' ') + 1);	//remove the 'class ' prefix
#else
        int status = 0;
		char* demangledName = abi::__cxa_demangle(typeid(TClass).name(), nullptr, nullptr, &status);
		UASSERT(status == 0);
		return Name(demangledName);
		::free(demangledName);
#endif
	}
	template<typename T> inline Name ZZGetEnumName()
	{
        return strchr(typeid(T).name(), ' ') + 1;	//remove the 'enum ' prefix
	}
	//////////////////////////////////////////////////////////////////////////
	/*
	template to convert any type to EPropertyType
	*/
#define IMP_PRPTRAIT_NONTPL(TypeName )\
	static const EMetaType Value = EMetaType::EPT_##TypeName;\
	static const char* GetTypeName() { return #TypeName; }\
	typedef TPropertyTypeExtract<void> TArg;

	//this struct helps to extract the required information of a type, will static assert if the type is not supported to be de|serialized
	//null specialization
	template < typename T /*the type of property*/, typename = void> struct TPropertyTypeExtract
	{
		struct _TArg
		{
			static const EMetaType Value = EMetaType::EPT_Unknown;
			static const char* GetTypeName() { return nullptr; }

			typedef _TArg TArg;
		};
		//the primary template argument, actually first argument, currently only one argument is needed for template's classes such as TArray, TObjectPtr, etc
		//thsi can be recursive so that TPropertyTypeExtract<TArray<TObjectPtr<MyObject>>>::TArg::TArg::GetTypeName() returns "MyObject"
		typedef _TArg TArg;

		static const EMetaType Value = EMetaType::EPT_Unknown;
		static const char* GetTypeName() { return nullptr; }
	};
	template<> struct TPropertyTypeExtract<bool>
	{
		IMP_PRPTRAIT_NONTPL(bool)
	};
	template<> struct TPropertyTypeExtract<char>
	{
		IMP_PRPTRAIT_NONTPL(int8)
	};
	template<> struct TPropertyTypeExtract<unsigned char>
	{
		IMP_PRPTRAIT_NONTPL(uint8)
	};
	template<> struct TPropertyTypeExtract<short>
	{
		IMP_PRPTRAIT_NONTPL(int16)
	};
	template<> struct TPropertyTypeExtract<unsigned short>
	{
		IMP_PRPTRAIT_NONTPL(uint16)
	};
	template<> struct TPropertyTypeExtract<int>
	{
		IMP_PRPTRAIT_NONTPL(int32)
	};
	template<> struct TPropertyTypeExtract<unsigned int>
	{
		IMP_PRPTRAIT_NONTPL(uint32)
	};
	template<> struct TPropertyTypeExtract<long>
	{
		IMP_PRPTRAIT_NONTPL(int32)
	};
	template<> struct TPropertyTypeExtract<unsigned long>
	{
		IMP_PRPTRAIT_NONTPL(uint32)
	};
	template<> struct TPropertyTypeExtract<long long>
	{
		IMP_PRPTRAIT_NONTPL(int64)
	};
	template<> struct TPropertyTypeExtract<unsigned long long>
	{
		IMP_PRPTRAIT_NONTPL(uint64)
	};
	template<> struct TPropertyTypeExtract<float>
	{
		IMP_PRPTRAIT_NONTPL(float)
	};
	template<> struct TPropertyTypeExtract<double>
	{
		IMP_PRPTRAIT_NONTPL(double)
	};
	//TArray
	template<typename T, typename TAlc> struct TPropertyTypeExtract<TArray<T, TAlc>>
	{
		typedef TPropertyTypeExtract<T> TArg;

		static const EMetaType Value = EMetaType::EPT_TArray;
		static const EMetaType ElementType = TArg::Value;

		static_assert(ElementType != EMetaType::EPT_Unknown, "unknown array element");
		static_assert(ElementType != EMetaType::EPT_TArray && ElementType != EMetaType::EPT_TStackArray, "array as array element is not supported");

		static const char* GetTypeName() { return ZZGetClassName<TArray<T>>(); }
	};

	/* #DEPRECATED
	//TStackArray
	template<typename T, size_t MaxLength> struct TPropertyTypeExtract<TStackArray<T, MaxLength / *its just a number to not get compile error* />>
	{
		typedef TPropertyTypeExtract<T> TArg;

		static const EMetaType Value = EMetaType::EPT_TStackArray;
		static const EMetaType ElementType = TArg::Value;

		static_assert(ElementType != EMetaType::EPT_Unknown, "unknown array element");
		static_assert(ElementType != EMetaType::EPT_TArray && ElementType != EMetaType::EPT_TStackArray, "array as array element is not supported");

		static const char* GetTypeName() { return ZZGetClassName<TStackArray<T,MaxLength>>(); }
	};
	*/
	//TSubClass
	template<typename T> struct TPropertyTypeExtract<TSubClass<T>>
	{
		typedef TPropertyTypeExtract<T> TArg;

		static const EMetaType Value = EMetaType::EPT_TSubClass;
		static const char* GetTypeName() { return ZZGetClassName<TSubClass<T>>(); }
	};
	//TObjectPtr
	template<typename T> struct TPropertyTypeExtract<TObjectPtr<T>>
	{
		typedef TPropertyTypeExtract<T> TArg;

		static const EMetaType Value = EMetaType::EPT_TObjectPtr;
		static const char* GetTypeName() { return ZZGetClassName<TObjectPtr<T>>(); }
	};
	//enum
	template<typename T> struct TPropertyTypeExtract<T, typename std::enable_if<std::is_enum<T>::value>::type>
	{
		typedef TPropertyTypeExtract<void> TArg;

		static const EMetaType Value = EMetaType::EPT_enum;
		static const char* GetTypeName() { return ZZGetEnumName<T>(); }
	};
	//Object*
	template<typename T> struct TPropertyTypeExtract<T, typename std::enable_if<std::is_pointer<T>::value && std::is_base_of<Object, typename std::remove_pointer<T>::type>::value>::type>
	{
		typedef TPropertyTypeExtract<void> TArg;

		static const EMetaType Value = EMetaType::EPT_ObjectPoniter;
		static const char* GetTypeName() { return ZZGetClassName<typename std::remove_pointer<T>::type>(); }
	};
	//MetaClass
	template<typename T> struct TPropertyTypeExtract<T, typename std::enable_if<TT_IsMetaClass<T>::value>::type>
	{
		typedef TPropertyTypeExtract<void> TArg;
		static const EMetaType Value = EMetaType::EPT_Class;

		static const char* GetTypeName() { return ZZGetClassName<T>(); }
	};



	struct ZZ_PropertyCheckResult
	{
		EMetaType		mPropertyType = EMetaType::EPT_Unknown;
		const char*		mPropertyTypeName = nullptr;	//eg: bool, UCore::Vec3, ..
		EMetaType		mArg0Type = EMetaType::EPT_Unknown;
		const char*		mArg0TypeName = nullptr;	//name of the TArray::ElementType, TSubClass::ClassType, TObjectPtr::ObjectType eg: int, UCore::Vec3, ...
		EMetaType		mArg0Arg0Type = EMetaType::EPT_Unknown;
		const char*		mArg0Arg0TypeName = nullptr; //in case TArray element is template. for example if property was TArray<TObjectPtr<MyObject>> this will be MyObject

		size_t			mSize = 0;
		size_t			mAlign = 0;

		template<typename T /*type of property*/> void Make()
		{
			typedef TPropertyTypeExtract<T> TResult;

			static_assert(TResult::Value != EMetaType::EPT_Unknown, "invalid property");

			mPropertyType = TResult::Value;
			mPropertyTypeName = TResult::GetTypeName();
			mArg0Type = TResult::TArg::Value;
			mArg0TypeName = TResult::TArg::GetTypeName();
			mArg0Arg0Type = TResult::TArg::TArg::Value;
			mArg0Arg0TypeName = TResult::TArg::TArg::GetTypeName();
			mSize = sizeof(T);
			mAlign = alignof(T);
		}
	};
	struct ZZ_PropertyRegParam : public ZZ_PropertyCheckResult
	{
		const char*				mName = nullptr;
		size_t					mByteOffset = 0;
		AttributePack			mAttributes;
	};




	UDECLARE_MEMBERFUNCTION_CHECKING(MetaSerialize, MetaSerialize, void, UCore::ByteSerializer&);
	UDECLARE_MEMBERFUNCTION_CHECKING(MetaDeserialize, MetaDeserialize, void, UCore::ByteDeserializer&);

	///////////new meta class function declaration here ^^^^^^^^^^^^^



	struct ZZ_ClassTypeCheckResult
	{
		Name									mClassName;	//eg UEngine::EntityStaticMesh
		Name									mParentClassName;	//eg UEngine::Entity
		bool									mHasParent = false;
		size_t									mClassSize = 0;
		size_t									mClassAlign = 0;

		FPMetaClassDefaultConstructor			mDefaulConstructor = nullptr;
		FPMetaClassDestructor					mDestructor = nullptr;
		FPMetaClassCopyConstructor				mCopyConstructor = nullptr;

		FPMetaSerialize							mMetaSerialize = nullptr;
		FPMetaDeserialize						mMetaDeserialize = nullptr;

		//#Note don't forget to forward new variables to ClassInfo
		//^^^^^^^^^^^^^^^^^ New meta class functions here ^^^^^^^^^^^^^^^^^^^^^^^

		template<typename TClass, typename TParent> void Make()
		{
			static_assert(std::is_class<TClass>::value, "must be a  class");
			//static_assert(std::is_default_constructible<TClass>::value && std::is_destructible<TClass>::value, "Class must be default constructible and destructible");

			mHasParent = !std::is_void<TParent>::value;
			mParentClassName = mHasParent ? ZZGetClassName<TParent>() : nullptr;
			mClassName = ZZGetClassName<TClass>();
			mClassSize = sizeof(TClass);
			mClassAlign = alignof(TClass);

			using IsDCT = typename std::is_default_constructible<TClass>;

			//if a class is not default constructible we set, C/D/CC tor to null, treat as  abstract class and cant be instantiated
			if (!IsDCT::value)
			{
				mDefaulConstructor = nullptr;
				mDestructor = nullptr;
				mCopyConstructor = nullptr;
			}
			else
			{
				typedef typename std::conditional<IsDCT::value, TClass, Void>::type TargetClassT;

				mDefaulConstructor = [](void* object) { new (object) TargetClassT(); };
				mDestructor = [](void* object) { ((TargetClassT*)object)->~TargetClassT(); };
				mCopyConstructor = [](void* object, const void* copyObject) { new (object) TargetClassT(*((const TargetClassT*)copyObject)); };

			}

			//meta functions
			{
				mMetaSerialize = UCLASS_GET_MEMBERFUNCTION(MetaSerialize, TClass);
				mMetaDeserialize = UCLASS_GET_MEMBERFUNCTION(MetaDeserialize, TClass);
			}

		}
	};


	struct ZZ_ClassRegParam : public ZZ_ClassTypeCheckResult
	{
		static const unsigned MAX_PROPERTY_COUNT = 128;

		const char*					mHeaderFileName = nullptr;
		const char*					mSourceFileName = nullptr;
		int							mHeaderFileLine = 0;
		int							mSourceFileLine = 0;
		ZZ_PropertyRegParam			mProperties[MAX_PROPERTY_COUNT];
		size_t						mNumProperty = 0;
		AttributePack				mAttributes;
	};

	//////////////////////////////////////////////////////////////////////////
	class UCORE_API MetaSys
	{
	public:
		const ClassInfo*	FindClass(Name className) const;
		const ClassInfo*	FindClass(Name::HashT classNameHash) const;
		const EnumInfo*		FindEnum(Name enumName) const;
		const FieldInfo*	FindType(Name name) const;
		const FieldInfo*	FindType(Name::HashT typeNameHash) const;
		void				GetSubClassesOf(const ClassInfo* theClass, TArray<const ClassInfo*>& out) const;

		void RegClass(const ZZ_ClassRegParam& regParam, ClassInfo*& outClassInfo);
		void UnregClass(ClassInfo*& inOutClas);

		const TArray<FieldInfo*>& GetAllTypes() const { return mRegisteredTypes; }

	private:

		TArray<FieldInfo*>	mRegisteredTypes;
	};

	UCORE_API MetaSys* GMetaSys();
};



#if 1
#define UCLASS_BEGIN_IMPL(Class, ...)\
	struct ZZZ_##Class\
	{\
		typedef Class TClass;\
		typedef typename Class::ParentT TParentClass;\
		UCore::ClassInfo* mClassInfo;\
		ZZZ_##Class()\
		{\
			mClassInfo = nullptr;\
			UCore::ZZ_ClassRegParam crp;\
			crp.Make<TClass, TParentClass>();\
			crp.mHeaderFileName = TClass::ZZFileName();\
			crp.mHeaderFileLine = TClass::ZZLineNumber();\
			crp.mSourceFileName = __FILE__;\
			crp.mSourceFileLine = __LINE__;\
			ZZPackAttribute(&crp.mAttributes, ##__VA_ARGS__);\
			crp.mNumProperty = 0;\



#if 1

#define UPROPERTY(Property, ...)\
			{\
				typedef decltype(TClass::Property) TPropertyType;\
				UCore::ZZ_PropertyRegParam& prp = crp.mProperties[crp.mNumProperty];\
				prp.Make<TPropertyType>(); \
				prp.mName = #Property;\
				prp.mByteOffset = offsetof(TClass, Property);\
				ZZPackAttribute(&prp.mAttributes, ##__VA_ARGS__);\
				crp.mNumProperty++;\
			}\

#else

#define UPROPERTY(Property, ...)

#endif // 0

#define UFUNCTION(Function, ...)



#define UCLASS_END_IMPL(Class)\
			UCore::GMetaSys()->RegClass(crp, mClassInfo); \
		}\
		~ZZZ_##Class()\
		{\
			UCore::GMetaSys()->UnregClass(mClassInfo);\
		}\
	} ZZZ_##Class##Instance;\
	const UCore::ClassInfo* Class::GetClassStatic() { return ZZZ_##Class##Instance.mClassInfo; }\

#else
#define UCLASS_BEGIN_IMPL(Class, ...)
#define UPROPERTY(Property, ...)
#define UCLASS_END_IMPL(Class)
#endif

#define UPROPERTY_NAME_EQUAL(PropertyInfoPtr, MemberVarible) PropertyInfoPtr->GetName() == #MemberVarible



