#include "Object.h"
#include "Meta.h"

namespace UCore
{
	UCLASS_BEGIN_IMPL(Object)
	UCLASS_END_IMPL(Object)
};

namespace UCore
{



	bool Object::IsBaseOf(const ClassInfo* baseClass) const
	{
		UASSERT(mClass);
		return mClass->IsBaseOf(baseClass);
	}

	//////////////////////////////////////////////////////////////////////////
	Object* Object::CopyConstructObjectHierarchy(bool bIncludingChildren, Object* parent /*= nullptr*/) const
	{
#if 0
		Object* cloned = SNew(this->GetClass(), parent, this);
		Object* srcChild = mObjectFirstChild;
		while (srcChild)
		{
			Object* dupChild = srcChild->ZZCloneObjects(cloned);
			srcChild = srcChild->GetObjectDown();
		}
		return cloned;
#endif // 

		UASSERT(mClass);
		UASSERT(mClass->HasCopyConstructor());

		void* objecMemory = ObjectUtility::AllocMemory(mClass);
		mClass->CallCopyConstructor(objecMemory, this);
		Object* clonedObject = (Object*)objecMemory;
		clonedObject->mClass = mClass;
		if (parent)
		{
			ObjectUtility::InitialAttachToParent(clonedObject, parent, false);
		}

		if (bIncludingChildren)
		{
			Object* child = this->mObjectFirstChild;
			while (child)
			{
				child->CopyConstructObjectHierarchy(true, clonedObject);
				child = child->mObjectDown;
			}
		}

		return clonedObject;
	}
	//////////////////////////////////////////////////////////////////////////
	Object* Object::Clone(bool bIncludingChildren) const
	{
		UASSERT(mClass);
		UASSERT(mClass->HasCopyConstructor());

		Object* clonedObjectRoot = CopyConstructObjectHierarchy(bIncludingChildren, nullptr);
		
		//calling post clone
		clonedObjectRoot->OnPostClone();
		clonedObjectRoot->ForEachChild(true, true, [](Object* child) {
			child->OnPostClone();
		});


		return clonedObjectRoot;

	}
	//////////////////////////////////////////////////////////////////////////
	Object* Object::SNew(const ClassInfo* pClass, Object* optParent, const Object* optCopyfrom)
	{
		if (pClass == nullptr)
		{
			ULOG_ERROR("null class");
			return nullptr;;
		}
		if (!pClass->IsDefaultConstructable())
		{
			ULOG_ERROR("[%] is not default constructible", pClass->GetName());
			return nullptr;
		}
		
		if (optCopyfrom)
		{
			if (!optCopyfrom->IsBaseOf(pClass))
			{
				optCopyfrom = nullptr;
				ULOG_WARN("[optCopyFrom] is not based on [pClass]. object is default constructed");
			}
		}

		Object* newObj = (Object*)ObjectUtility::AllocMemory(pClass);

		

		if (optCopyfrom)
			pClass->CallCopyConstructor(newObj, optCopyfrom);
		else
			pClass->CallDefaultConstructor(newObj);
		
		newObj->mClass = pClass;

		ObjectUtility::InitialAttachToParent(newObj, optParent, true);
		
		if (optCopyfrom)
			newObj->OnPostClone();
		else
			newObj->OnPostConstruction();

		return newObj;
	}

	Object::~Object()
	{
		Object* child = mObjectFirstChild;
		while (child)
		{
			Object* childDown = child->mObjectDown;

			child->~Object();
			ObjectUtility::FreeMemory(child);

			child = childDown;
		}
	}

	void Object::SDelete(Object* pObject)
	{
		ULOG_MESSAGE("");
		UASSERT(pObject);

		pObject->ObjectDetach();
		pObject->OnDestruction();

		pObject->~Object();	//object has virtual DTor so we don't need to call its DTor trough ClassInfo
		
		ObjectUtility::FreeMemory(pObject);
	}





	void ObjectUtility::DetachObject(Object* object)
	{
		if (object->mObjectParent)
		{
			if (object->mObjectParent->mObjectFirstChild == object) //is first child
			{
				UASSERT(object->mObjectUp, "its first child must point to last child");

				if (object->mObjectDown == nullptr) // is first and last child ?
				{
					object->mObjectParent->mObjectFirstChild = nullptr;
					object->mObjectUp = nullptr;
					object->mObjectParent = nullptr;
					return;
				}
				else //is first child and not last child ?
				{
					object->mObjectDown->mObjectUp = object->mObjectUp;
					object->mObjectParent->mObjectFirstChild = object->mObjectDown;

					object->mObjectDown = nullptr;
					object->mObjectUp = nullptr;
					object->mObjectParent = nullptr;
					return;
				}
			}
			else  
			{
				if (object->mObjectDown == nullptr) //is last child and not first child?
				{
					object->mObjectParent->mObjectFirstChild->mObjectUp = object->mObjectUp;
					object->mObjectUp->mObjectDown = nullptr;
					object->mObjectUp = nullptr;
				}
				else //nor first nor last
				{
					object->mObjectDown->mObjectUp = object->mObjectUp;
					object->mObjectUp->mObjectDown = object->mObjectDown;

					object->mObjectDown = nullptr;
					object->mObjectUp = nullptr;
				}

				object->mObjectParent = nullptr;
			}
		}

	}

	void ObjectUtility::InitialAttachToParent(Object* obj, Object* parent, bool bTop)
	{
		UASSERT(obj);

		if (parent)
		{
			obj->mObjectParent = parent;

			if (parent->mObjectFirstChild) //has parent any child?
			{
				if (bTop)
				{
					obj->mObjectUp = parent->mObjectFirstChild->mObjectUp;
					obj->mObjectDown = parent->mObjectFirstChild;

					parent->mObjectFirstChild->mObjectUp = obj;
					parent->mObjectFirstChild = obj;
				}
				else
				{
					Object* lastChild = parent->mObjectFirstChild->mObjectUp;
					obj->mObjectUp = lastChild;
					obj->mObjectDown = nullptr;

					lastChild->mObjectDown = obj;

					parent->mObjectFirstChild->mObjectUp = obj;

				}
			}
			else
			{
				obj->mObjectDown = nullptr;
				obj->mObjectUp = obj;	//the last object is itself
				parent->mObjectFirstChild = obj;
			}
		}
		else
		{
			obj->mObjectParent = nullptr;
			obj->mObjectUp = nullptr;
			obj->mObjectDown = nullptr;
		}

	}

	void ObjectUtility::InitialPutTo(Object* object, Object* target, bool bBefore)
	{
		UASSERT(object && target);
		UASSERT(target->mObjectParent);

		ULOG_FATAL("not implemented yet");
	}

	void* ObjectUtility::AllocMemory(const ClassInfo* pObjectClass)
	{
		UASSERT(pObjectClass);
		UASSERT(pObjectClass->IsBaseOf<Object>());
		return MemAllocAlignedZero(pObjectClass->GetSize(), pObjectClass->GetAlign());
	}

	void ObjectUtility::FreeMemory(Object* pObject)
	{
		UASSERT(pObject);
		return MemFreeAligned(pObject);
	}

	Object* ObjectUtility::NewDefault(const ClassInfo* pClass, Object* optParent, bool bAtttachAsFirstChild)
	{
		if (pClass == nullptr)
		{
			ULOG_ERROR("null class");
			return nullptr;;
		}
		if (!pClass->IsDefaultConstructable())
		{
			ULOG_ERROR("[%] is not default constructible", pClass->GetName());
			return nullptr;
		}

		Object* newObject = (Object*)AllocMemory(pClass);
		UASSERT(newObject);
		pClass->CallDefaultConstructor(newObject);
		newObject->mClass = pClass;
		ObjectUtility::InitialAttachToParent(newObject, optParent, bAtttachAsFirstChild);
		UASSERT(newObject->mClass);
		return newObject;
	}

	Name PropertyChangedEvent::GetPropertyName() const
	{
		return (mProperty ? mProperty->GetName() : Name());
	}

};