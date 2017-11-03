#pragma once


#include "Base.h"

namespace UCore
{
	template<typename T> class TSPtr;
	template<typename T> class TWPtr;

	//this is newed by TBaseSmart::GetWeakData and will be deleted when mObject && mRefCount are zero
	//newing and deleting is done by a FreeList allocator fast enough
	struct UCORE_API WeakRefrenceData
	{
		void*		mObject;	//pointer to the object that TWeakPtr is pointing to, null means the object has been deleted
		unsigned	mRefCount;	//number of alive TWPtr

		WeakRefrenceData(void* obj, unsigned initialRefCount = 0) : mObject(obj), mRefCount(initialRefCount) 
		{
		}
		~WeakRefrenceData();

		inline void Inc() { mRefCount++; }
		//decrease the refcount and delete this if needed
		void Dec();

		//return the null weak, increasing or decreasing of its refcount has no effect
		static WeakRefrenceData* GetNull();

		static void* operator new (size_t);
		static void operator delete(void*);
	};


	class IWeakBase
	{
	public:
		IWeakBase() : mWeakData(nullptr) {}
		IWeakBase(const IWeakBase& copy) : mWeakData(nullptr) {}

		//return pointer to the WeakData of this object, allocate if needed, doesn't increase the refcount
		WeakRefrenceData* GetWeakData() const
		{
			if (mWeakData == nullptr)
				mWeakData = new WeakRefrenceData((void*)this, 0);

			return mWeakData;
		}
		virtual ~IWeakBase()
		{
			if (mWeakData)
			{
				UASSERT(mWeakData->mObject);
				mWeakData->mObject = nullptr;
				if (mWeakData->mRefCount == 0)	//there is no TWeakPtr alive?
				{
					if (mWeakData != WeakRefrenceData::GetNull())
						delete mWeakData;
				}
				mWeakData = nullptr;
			}
		}
	private:
		mutable WeakRefrenceData*	mWeakData;
	};


	/*
		inherit from this then  TSPtr and TWPtr can handle your class
	*/
	class ISmartBase : public IWeakBase
	{
	public:
		ISmartBase() : mRefCount(0) {}
		virtual ~ISmartBase()
		{
			UASSERT(mRefCount == 0);
		}

		inline void AddRef()
		{
			mRefCount++;
		}
		inline void Release()
		{
			UASSERT(mRefCount != 0);
			mRefCount--;
			if (mRefCount == 0)
				delete this;
		}
	private:
		unsigned					mRefCount;
	};



	//////////////////////////////////////////////////////////////////////////weak pointer
	template<typename T> class TWPtr
	{
	public:
		using ClassT = T;

		TWPtr()
		{
			mWeakData = WeakRefrenceData::GetNull();
		}
		TWPtr(std::nullptr_t)
		{
			mWeakData = WeakRefrenceData::GetNull();
		}
		TWPtr(const TWPtr& copy)
		{
			mWeakData = copy.mWeakData;
			mWeakData->Inc();
		}
		TWPtr(const T* ptr)
		{
			if (ptr) 
			{
				mWeakData = ptr->GetWeakData();
				mWeakData->Inc();
			}
			else
			{
				mWeakData = WeakRefrenceData::GetNull();
			}
			
		}
		TWPtr& operator = (const TWPtr& wptr)
		{
			if(wptr.mWeakData != mWeakData)
			{
				wptr.mWeakData->Inc();
				mWeakData->Dec();
				mWeakData = wptr.mWeakData;
			}
			return *this;
		}
		TWPtr& operator = (const T* bptr)
		{
			if (bptr)
			{
				WeakRefrenceData* newWeak = bptr->GetWeakData();
				newWeak->Inc();
				mWeakData->Dec();
				mWeakData = newWeak;
			}
			else
			{
				mWeakData->Dec();
				mWeakData = WeakRefrenceData::GetNull();
			}
			return *this;
		}
		TWPtr& operator = (std::nullptr_t)
		{
			mWeakData->Dec();
			mWeakData = WeakRefrenceData::GetNull();
			return *this;
		}
		~TWPtr()
		{
			mWeakData->Dec();
			mWeakData = nullptr;
		}

		bool		IsValid() const { return mWeakData->mObject != nullptr; }
		T*			Get() const { return (T*)mWeakData->mObject; }

		operator bool() const { return mWeakData->mObject != nullptr; }
		operator T* () const { return (T*)mWeakData->mObject; }

		template<typename U> operator TWPtr<U>() const { return TWPtr<U>(Get()); }

		bool operator == (const TWPtr& wptr) const
		{
			return mWeakData == wptr.mWeakData;
		}
		bool operator != (const TWPtr& wptr) const
		{
			return mWeakData != wptr.mWeakData;
		}
		T* operator -> () const { return (T*)mWeakData->mObject; }

	private:
		WeakRefrenceData*	mWeakData;	//always valid
	};
	

	//////////////////////////////////////////////////////////////////////////shared pointer
	template<typename T> class TSPtr
	{
	public:
		using ClassT = T;

		TSPtr()
		{
			mPtr = nullptr;
		}
		TSPtr(std::nullptr_t)
		{
			mPtr = nullptr;
		}
		~TSPtr()
		{
			if (mPtr) mPtr->Release();
			mPtr = nullptr;
		}
		TSPtr(T* ptr)
		{
			if (ptr)
			{
				mPtr = ptr;
				mPtr->AddRef();
			}
			else
			{
				mPtr = nullptr;
			}
		}
		TSPtr(const TSPtr& copy)
		{
			mPtr = copy.mPtr;
			if (mPtr) mPtr->AddRef();
		}

		TSPtr& operator = (const TSPtr& other)
		{
			return this->operator=(other.mPtr);
		}
		TSPtr& operator = (T* ptr)
		{
			if (ptr)
			{
				ptr->AddRef();
				if (mPtr) mPtr->Release();
				mPtr = ptr;
			}
			else
			{
				if (mPtr) mPtr->Release();
				mPtr = nullptr;
			}
			return *this;
		}

		TWPtr<T> GetWeak() const
		{
			return TWPtr<T>(mPtr);
		}
		T* Get() const { return mPtr; }

		bool IsValid() const { return mPtr != nullptr; }

		operator bool() const { return mPtr != nullptr; }
		operator T* () const { return mPtr; }

		T* operator -> () const { return mPtr; }

		template<typename U> operator TSPtr<U>() const { return TSPtr<U>(mPtr); }

		bool operator == (const TSPtr& other) const { return mPtr == other.mPtr; }
		bool operator != (const TSPtr& other) const { return mPtr != other.mPtr; }

	private:
		T* mPtr;
	};

	template<typename T, typename U> TSPtr<T> PtrStaticCast(const TSPtr<U>& sptr)
	{
		auto p = static_cast<T*>(sptr.Get());
		return TSPtr<T>(p);
	}
	template<typename T, typename U> TWPtr<T> PtrStaticCast(const TWPtr<U>& sptr)
	{
		auto p = static_cast<T*>(sptr.Get());
		return TWPtr<T>(p);
	}

	template<typename T, typename U> TSPtr<T> PtrDynamicCast(const TSPtr<U>& sptr)
	{
		auto p = dynamic_cast<T*>(sptr.Get());
		return TSPtr<T>(p);
	}
	template<typename T, typename U> TWPtr<T> PtrDynamicCast(const TWPtr<U>& sptr)
	{
		auto p = dynamic_cast<T*>(sptr.Get());
		return TWPtr<T>(p);
	}
};