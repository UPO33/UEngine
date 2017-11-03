#pragma once

#include "Name.h"
#include "Array.h"
#include "SmartPointers.h"

#ifdef GetClassInfo
#undef  GetClassInfo
#endif



namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class Object;
	class ClassInfo;
	class ObjectRefData;
	class ObjectSys;
	class MetaSys;
	class PropertyInfo;
	
	struct UCORE_API PropertyChangedEvent
	{
		const PropertyInfo*		mProperty;

		PropertyChangedEvent(const PropertyInfo* intendedProperty = nullptr) : mProperty(intendedProperty) {}

		//return the changed property or is about to change
		const PropertyInfo* GetProperty() const { return mProperty; }
		Name				GetPropertyName() const;
	};
	
	enum EObjectFlags
	{
		EOF_EditorCreated,
		EOF_CodeCreated,
		EOF_CopyConstructed,	//means objects is constructed using copy constructor otherwise default constructor
	};

	class UCORE_API ObjectUtility
	{
	public:

		static void DetachObject(Object*);
		//@bTop		whether attach to parent as first child or last child
		static void InitialAttachToParent(Object* object, Object* parent, bool bTop);
		//@target must have parent
		//whether put this object before or after the @target 
		static void InitialPutTo(Object* object, Object* target, bool bBefore);

		//#Future: should use custom and thread safe allocator for better performance
		static void* AllocMemory(const ClassInfo* pObjectClass);
		static void FreeMemory(Object* pObject);
		static Object* NewDefault(const ClassInfo* pObjectClass, Object* optParent, bool bAtttachAsFirstChild);
		
	};

	//////////////////////////////////////////////////////////////////////////
	class UCORE_API Object : public IWeakBase
	{
		UCLASS(Object)

			template<typename T> friend class TObjectPtr;
		friend ObjectSys;
		friend MetaSys;
		friend ObjectUtility;

		Object() :
			mClass(nullptr),
			mObjectDown(nullptr), mObjectUp(nullptr), mObjectFirstChild(nullptr), mObjectParent(nullptr)
		{

		}
		Object(const Object& copy) :
			mClass(nullptr),
			mObjectDown(nullptr), mObjectUp(nullptr), mObjectFirstChild(nullptr), mObjectParent(nullptr)
		{
		}
		~Object();

		const ClassInfo*	GetClass() const { return mClass; }
		bool				IsBaseOf(const ClassInfo* baseClass) const;

		virtual void OnPrePropertyChange(const PropertyChangedEvent&) {}
		virtual void OnPostPropertyChange(const PropertyChangedEvent&) {}
		virtual void OnMetaSerialize(ByteSerializer&) {}
		virtual void OnMetaDeserialize(ByteDeserializer&) {}
		//is called after copy constructor
		virtual void OnPostClone() {}
		//is called after default constructor
		virtual void OnPostConstruction() {}
		virtual void OnPostLoad() {}
		virtual void OnMessage(int id, void* data) {}

		//this function just allocates and copy constructs the object hierarchy
		Object* CopyConstructObjectHierarchy(bool bIncludingChildren = true, Object* parent = nullptr) const;
		//clones this object and return it, the resulting object is not no parent
		Object* Clone(bool bIncludingChildren = true) const;

		template<typename T> const T*	Cast() const
		{
			if (IsBaseOf(T::GetClassStatic())) return (const T*) this;
			return nullptr;
		}
		template<typename T> T*			Cast()
		{
			if (IsBaseOf(T::GetClassStatic())) return (T*) this;
			return nullptr;
		}
		template<typename T> const T*	CastSure() const
		{
			if (IsBaseOf(T::GetClassStatic())) return (const T*) this;
			UASSERT(false);
			return nullptr;
		}
		template<typename T> T*			CastSure()
		{
			if (IsBaseOf(T::GetClassStatic())) return (T*) this;
			UASSERT(false);
			return nullptr;
		}
		//@bIncludingSubChildren	whether to call @proc on sub-children or not
		//@bContinuousInSameColumn	if bIncludingSubChildren this indicates iterating children continuously or recursively
		template<typename TLambda> void ForEachChild(const bool bIncludingSubChildren, const bool bContinuousInSameColumn, TLambda proc)
		{
			if (bIncludingSubChildren && bContinuousInSameColumn)
			{
				Object* child = mObjectFirstChild;
				while (child)
				{
					proc(child);
					child = child->mObjectDown;
				}

				child = mObjectFirstChild;
				while (child)
				{
					child->ForEachChild(true, true, proc);
					child = child->mObjectDown;
				}
			}
			else
			{
				Object* child = mObjectFirstChild;
				while (child)
				{
					proc(child);
					if (bIncludingSubChildren)
						child->ForEachChild(true, false, proc);
					child = child->mObjectDown;
				}
			}
		}
		//returns true this object is child of @parent, also false if @parent is null or this
		bool IsSubsetOf(const Object* parent) const
		{
			if (parent)
			{
				Object* iterParent = mObjectParent;
				while (iterParent)
				{
					if (iterParent == parent)
						return true;

					iterParent = iterParent->mObjectParent;
				}
			}
			return false;
		}
		//length of longest path from this object to the root
		unsigned GetObjectHeight() const
		{
			unsigned index = 0;
			Object* parent = mObjectParent;
			while (parent)
			{
				index++;
				parent = parent->mObjectParent;
			}
			return index;
		}
		//allocates a new object of @pClass
		//if @optCopyfrom is set the new object is crated using copy constructor, @optCopyFrom's class must be based on @pClass
		static Object*	SNew(const ClassInfo* pClass, Object* optParent = nullptr, const Object* optCopyFrom = nullptr);
		static void		SDelete(Object* pObject);

		//returns the parent of this object if any
		Object* GetObjectParent() const { return mObjectParent; }
		//returns pointer to the first child of this object, u can loop over the children of a object by the following code :
		//Object* child = object->GetObjectFirstChild();
		//while(child)
		//{
		//	child = child->GetObjectDown();
		//}
		Object* GetObjectFirstChild() const { return mObjectFirstChild; }
		//return the top object of this object if any
		Object* GetObjectUp() const { return mObjectUp; }
		//returns the bottom object of this object null 
		Object* GetObjectDown() const { return mObjectDown; }

		//returns true if this object is the first child of its parent
		bool IsFirstChild() const
		{
			return mObjectParent && mObjectParent->mObjectFirstChild == this;
		}
		//returns true if this object is last child of its parent
		bool IsLastChild() const
		{
			return mObjectParent && mObjectDown == nullptr;
		}
		Object* GetObjectLastChild() const
		{
			if (mObjectFirstChild)
				return mObjectFirstChild->mObjectUp;

			return nullptr;
		}

		void ObjectDetach()
		{
			ObjectUtility::DetachObject(this);
		}
		//attach this object to the top of @newParant's children so that this will be her first child, 
		//if newParent is null detach this from current parent
		//@bTop		whether attach to parent as first child or last child
		virtual void ObjectAattachTo(Object* newParent, bool bTop = false)
		{
			if (mObjectParent == newParent) return;

			if (newParent == nullptr)
			{
				ObjectDetach();
			}
			else
			{
				ObjectUtility::DetachObject(this);
				ObjectUtility::InitialAttachToParent(this, newParent, bTop);
			}
		}


		//is called as a pass before Destructor, recursive, derived must call parent at th end
		virtual void OnDestruction()
		{
			Object* child = mObjectFirstChild;
			while (child)
			{
				child->OnDestruction();

				child = child->mObjectDown;
			}
		}
		//allocates the same object hierarchy, just calls CCTor

		virtual Name GetName() const { return Name(); }
		virtual void DebugPrint() {}

#ifdef _DEBUG
		void SetObjectDebugStr(const char* cstr) { mDebugStr = cstr; }
		const char* GetObjectDebugStr() const { return mDebugStr; }
#else
		void SetObjectDebugStr(const char* cstr) { }
		const char* GetObjectDebugStr() const { return nullptr; }
#endif

	private:

#ifdef _DEBUG
		const char*			mDebugStr = nullptr;
#endif

		const ClassInfo*	mClass;
		//pointer to parent of this object if any
		Object*				mObjectParent;
		//pointer to first child of this object if any, this->mObjectFirstChild->mObjectUp points to the last child of this object
		Object*				mObjectFirstChild;
		//pointer to the the object top of this object, if this object is first child it points to last child of its parent
		Object*				mObjectUp;
		//pointer to the bottom child. if this object is the last one its null
		Object*				mObjectDown;

		unsigned			mObjectFlags;
		

	};


	template<typename TObjectClass> TObjectClass* NewObject(const ClassInfo* pObjectClass = TObjectClass::GetClassStatic(), Object* parent = nullptr, const Object* copyFrom = nullptr)
	{
		return (TObjectClass*)Object::SNew(pObjectClass, parent, copyFrom);
	}
	inline void DeleteObject(Object* object)
	{
		Object::SDelete(object);
	}

	template<typename T> T* UCast(Object* object)
	{
		if (object) return object->Cast<T>();
		return nullptr;
	}
	template<typename T> const T* UCast(const Object* object)
	{
		if (object) return object->Cast<T>();
		return nullptr;
	}
	template<typename T> T* UCastSure(Object* object)
	{
		T* ret = UCast<T>(object);
		UASSERT(ret);
		return ret;
	}
	template<typename T> const T* UCastSure(const Object* object)
	{
		const T* ret = UCast<T>(object);
		UASSERT(ret);
		return ret;
	}
};