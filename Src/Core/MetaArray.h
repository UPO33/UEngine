#pragma once

#include "Array.h"
#include "Meta.h"

namespace UCore
{
	class UCORE_API MetaArrayView
	{
	public:
		//the properties that a TArray has
		struct ArrayData
		{
			size_t				mLength;
			size_t				mCapacity;
			ArrayAllocBase		mAllocator;
		};

		MetaArrayView(void* pointerToTArray, const TriTypeData& elementType)
			: mArrayPtr((ArrayData*)pointerToTArray), mElementType(elementType) {}

		size_t Length() const;
		size_t Capacity() const;
		size_t GetElementTypeSize() const;
		size_t GetElementTypeAlign() const;

		size_t		AddDefault(size_t count);
		void		RemoveAll();
		//returns pointer to the element at 'index', null if out of range 
		void*		GetElementAt(size_t index) const;
		void		RemoveAtShift(size_t index);
		bool		IsIndexValid(size_t index) const;
		char*		Elements() const;

	private:
		void		IncCapacity(size_t count);
		void		CallCTor(size_t begin, size_t count);
		void		CallDTor(size_t begin, size_t count);
		


		ArrayData* mArrayPtr;
		const TriTypeData&	mElementType;

	};
};