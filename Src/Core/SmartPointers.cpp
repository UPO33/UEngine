#include "SmartPointers.h"
#include "Allocators.h"



namespace UCore
{
	UCORE_API WeakRefrenceData* gNullWeakRefrence = nullptr;

	UCORE_API FreeListAllocator* GWeakRefrenceDataAllocator()
	{
		static TInstance<FreeListAllocator> Ins(sizeof(WeakRefrenceData), 256);
		return Ins;
	}

	WeakRefrenceData::~WeakRefrenceData()
	{
		ULOG_MESSAGE("");
		if(gNullWeakRefrence != this)
		{
			UASSERT(mRefCount == 0);
			UASSERT(mObject == nullptr);
		}
	}

	void WeakRefrenceData::Dec()
	{
		mRefCount--;
		if (mRefCount == 0)
		{
			if (mObject == nullptr) //object is deleted then delete this too?
			{
				if (this != GetNull())	//null weak must not be deleted
					delete this;
			}
		}
	}

	WeakRefrenceData* WeakRefrenceData::GetNull()
	{
		if (gNullWeakRefrence == nullptr)
			gNullWeakRefrence = new WeakRefrenceData(nullptr, 0xFFffFF);

		return gNullWeakRefrence;
	}


	void* WeakRefrenceData::operator new(size_t size)
	{
		UASSERT(size == sizeof(WeakRefrenceData));
		return GWeakRefrenceDataAllocator()->Alloc();
	}


	void WeakRefrenceData::operator delete(void* ptr)
	{
		UASSERT(ptr);
		GWeakRefrenceDataAllocator()->Free(ptr);
	}

};