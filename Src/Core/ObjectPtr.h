#pragma once

#include "Base.h"
#include "SmartPointers.h"
#include "Object.h"

namespace UCore
{

	//////////////////////////////////////////////////////////////////////////
	template<typename TObject> class TObjectPtr
	{
	public:
		using ObjectType = TObject;
		//static_assert(std::is_base_of<Object, typename TObject>::value, "TObject is not Object class");

		TObjectPtr()
		{
			mRefData = WeakRefrenceData::GetNull();
		}
		TObjectPtr(std::nullptr_t)
		{
			mRefData = WeakRefrenceData::GetNull();
		}
		TObjectPtr(TObject* object)
		{
			if (object)
			{
				mRefData = object->GetWeakData();
				mRefData->Inc();
			}
			else
			{
				mRefData = WeakRefrenceData::GetNull();
			}
		}
		TObjectPtr(const TObjectPtr& other)
		{
			mRefData = other.mRefData;
			mRefData->Inc();
		}
		TObjectPtr& operator = (const TObjectPtr& other)
		{
			other.mRefData->Inc();
			mRefData->Dec();
			mRefData = other.mRefData;
			return *this;
		}
		TObjectPtr& operator = (TObject* object)
		{
			if (object)
			{
				auto tmp = object->GetWeakData();
				tmp->Inc();
				mRefData->Dec();
				mRefData = tmp;
			}
			else
			{
				mRefData->Dec();
				mRefData = WeakRefrenceData::GetNull();
			}
			return *this;
		}
		TObjectPtr& operator = (std::nullptr_t)
		{
			mRefData->Dec();
			mRefData = WeakRefrenceData::GetNull();
			return *this;
		}
		~TObjectPtr()
		{
			mRefData->Dec();
			mRefData = nullptr;
		}
		operator bool() const { return mRefData->mObject != nullptr; }
		operator TObject* () { return (TObject*)mRefData->mObject; }
		operator const TObject* () const { return (TObject*)mRefData->mObject; }
		TObject* operator -> () const { return (TObject*)mRefData->mObject; }
		TObject* Get() const { return (TObject*)mRefData->mObject; }

		bool operator == (const Object* obj) const { return ((Object*)mRefData->mObject) == obj; }
		bool operator != (const Object* obj) const { return ((Object*)mRefData->mObject) != obj; }

		bool operator == (const TObjectPtr& objptr) const
		{
			return mRefData == objptr.mRefData;
		}
		bool operator != (const TObjectPtr& objptr) const
		{
			return mRefData != objptr.mRefData;
		}
	private:
		WeakRefrenceData* mRefData;
	};



 	typedef TObjectPtr<Object> ObjectPtr;

};
