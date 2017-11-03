#include "MetaArray.h"




namespace UCore
{
	//////////////////////////////////
	size_t MetaArrayView::Length() const
	{
		return mArrayPtr->mLength;
	}

	size_t MetaArrayView::Capacity() const
	{
		return mArrayPtr->mCapacity;
	}

	size_t MetaArrayView::GetElementTypeSize() const
	{
		switch (mElementType.GetType())
		{
		case EMetaType::EPT_Unknown:
		case EMetaType::EPT_TArray:
		{
			ULOG_FATAL("Invalid element type");
			return 0;
		}

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
		case EMetaType::EPT_enum: return 4;
		case EMetaType::EPT_TObjectPtr: return sizeof(TObjectPtr<Object>);
		case EMetaType::EPT_TSubClass: return sizeof(TSubClass<Object>);
		case EMetaType::EPT_ObjectPoniter: return sizeof(Object*);
		case EMetaType::EPT_Class:
		{
			UASSERT(mElementType.GetPtr());
			return mElementType.GetPtr()->GetSize();
		}
		}
		return 0;
	}


	size_t MetaArrayView::GetElementTypeAlign() const
	{
		switch (mElementType.GetType())
		{
		case EMetaType::EPT_Unknown:
		case EMetaType::EPT_TArray:
		{
			ULOG_FATAL("Invalid element type");
			return 0;
		}

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
		case EMetaType::EPT_enum: return 4;
		case EMetaType::EPT_TObjectPtr: return alignof(TObjectPtr<Object>);
		case EMetaType::EPT_TSubClass: return alignof(TSubClass<Object>);
		case EMetaType::EPT_ObjectPoniter: return alignof(Object*);
		case EMetaType::EPT_Class:
		{
			UASSERT(mElementType.GetPtr());
			return mElementType.GetPtr()->GetAlign();
		}
		}
		return 0;
	}

	void MetaArrayView::IncCapacity(size_t count)
	{
		if (count == 0) return;

		if (mArrayPtr->mLength) //already we have elements?
		{
			auto capcity = mArrayPtr->mCapacity;
			mArrayPtr->mCapacity += count;
			mArrayPtr->mAllocator.VRealloc(capcity, capcity + count);
			
		}
		else
		{
			mArrayPtr->mCapacity = count;
			mArrayPtr->mAllocator.VAlloc(count * GetElementTypeSize());
		}
	}

	size_t MetaArrayView::AddDefault(size_t count)
	{
		if (Capacity() < (Length() + count)) IncCapacity(count);

		auto preLength = mArrayPtr->mLength;
		mArrayPtr->mLength += count;
		CallCTor(preLength, count);
		return preLength;
	}

	void MetaArrayView::RemoveAll()
	{
		CallDTor(0, mArrayPtr->mLength);
		mArrayPtr->mLength = 0;
	}

	void* MetaArrayView::GetElementAt(size_t index) const
	{
		if (index >= Length()) return nullptr;

		return Elements() + (GetElementTypeSize() * index);
	}

	void MetaArrayView::RemoveAtShift(size_t index)
	{
		if (index < Length())
		{
			CallDTor(index, 1);
			MemMove(GetElementAt(index), GetElementAt(index + 1), (Length() - (index + 1)) * GetElementTypeSize());

			mArrayPtr->mLength--;
		}
	}

	bool MetaArrayView::IsIndexValid(size_t index) const
	{
		return index < Length();
	}

	void MetaArrayView::CallCTor(size_t begin, size_t count)
	{
		UASSERT(begin + count <= Length());

		//zeroing first
		MemZero(GetElementAt(begin), count * GetElementTypeSize());

		for (size_t iElement = begin; iElement < (begin + count); iElement++)
		{
			void* element = GetElementAt(iElement);
			UASSERT(element);

			if (mElementType.GetType() == EMetaType::EPT_Class)
			{
				const ClassInfo* elementClass = Cast<ClassInfo>(mElementType.GetPtr());
				UASSERT(elementClass);

				if (elementClass->HasDefaultConstructor()) elementClass->CallDefaultConstructor(element);
				return;
			}
			if (mElementType.GetType() == EMetaType::EPT_TObjectPtr)
			{
				new (element) ObjectPtr();
				return;
			}
			if (mElementType.GetType() == EMetaType::EPT_TSubClass)
			{
			}
		}
	}

	void MetaArrayView::CallDTor(size_t begin, size_t count)
	{
		UASSERT(begin + count <= Length());

		for (size_t iElement = begin; iElement < (begin + count); iElement++)
		{
			void* element = GetElementAt(iElement);
			UASSERT(element);

			if (mElementType.GetType() == EMetaType::EPT_Class)
			{
				const ClassInfo* elementClass = Cast<ClassInfo>(mElementType.GetPtr());
				UASSERT(elementClass);
				if (elementClass->HasDestructor()) elementClass->CallDestructor(element);
				return;
			}

			if (mElementType.GetType() == EMetaType::EPT_TObjectPtr)
			{
				((ObjectPtr*)element)->~ObjectPtr();
				return;
			}

			if (mElementType.GetType() == EMetaType::EPT_TSubClass)
			{

			}
		}
	}

	char* MetaArrayView::Elements() const
	{
		return static_cast<char*>(mArrayPtr->mAllocator.VData());
	}

};
