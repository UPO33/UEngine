#pragma once

#include "Base.h"
#include "Memory.h"

namespace UCore
{
	typedef size_t ArrayIndexT;

	static const size_t INVALID_INDEX = 0xFFffFFff;

#pragma region global functions
	//removes the intended elements of a array and keep the order of elements
	//this function is fast enough in case over 10% of the elements are supposed to be removed, more percent more performance
	//if TElement's ctor dtor must be handled properly use 'ArrayConditionalRemove()' instead
	template<typename TElement, typename TLength, typename TLambda> void ArrayConditionalRemovePOD(TElement* arrayElements, TLength& arrayLength, TLambda proc)
	{
		TLength iWriteElement = 0;
		for (TLength iElement = 0; iElement < arrayLength; iElement++)
		{
			if (proc(arrayElements[iElement]))
			{
				continue;
			}
			else
			{
				//@note: we don't need to check the equality of iWriteElement and iElement for better performance
				//when they are same the memory would be same so that it seems CPU actually doesn't do any MOV
				arrayElements[iWriteElement] = arrayElements[iElement];
				iWriteElement++;
			}
		}
		arrayLength = iWriteElement;
	};
	template<typename TElement, typename TLength, typename TLambda> void ArrayConditionalRemove(TElement* arrayElements, TLength& arrayLength, TLambda proc)
	{
		TLength iWriteElement = 0;
		for (TLength iElement = 0; iElement < arrayLength; iElement++)
		{
			if (proc(arrayElements[iElement]))
			{
				arrayElements[iElement].~TElement();
				continue;
			}
			else
			{
				if (iWriteElement != iElement)
				{
					new (arrayElements + iWriteElement) TElement(arrayElements[iElement]);
					arrayElements[iElement].~TElement();
				}

				iWriteElement++;
			}
		}
		arrayLength  = iWriteElement;
	};
	template<typename TElement, typename TIndex> void ArrayRemoveAtSwap(TElement* elements, TIndex& length, TIndex index)
	{
		UASSERT(length && index < length);
		length--;
		if (index != length)
			elements[index] = std::move(elements[length]);
		else
			elements[index].~TElement();
	}
	//only calls destructor on removing elements
	template<typename TElement, typename TIndex> void ArrayRemoveShift(TElement* elements, TIndex& length, TIndex removeIndex, TIndex removeCount)
	{
		UASSERT(removeIndex < length && removeCount > 0);
		UASSERT(removeIndex + removeCount <= length);
		TIndex n = removeIndex + removeCount;
		//calling destructor
		for (TIndex i = removeIndex; i < n; i++)
			(elements + i)->~TElement();

		MemMove(elements + removeIndex, elements + n, (length - n) * sizeof(TElement));
		length -= removeCount;
	}
	template<typename TElement, typename TIndex> TIndex ArrayFindElement(const TElement* elements, TIndex length, const TElement& elementToFind)
	{
		for (TIndex i = 0; i < length; i++)
		{
			if (elements[i] == elementToFind)
				return i;
		}
		return INVALID_INDEX;
	}
	template<typename TElement, typename TIndex> void ArrayReverse(TElement* elements, TIndex length)
	{
		for (TIndex iIndex = 0; iIndex < length / 2; iIndex++)
		{
			auto temp = elements[length - iIndex - 1];
			elements[length - iIndex - 1] = elements[iIndex];
			elements[iIndex] = temp;
		}
	}
#pragma  endregion



	//base allocator class for TArray
	struct ArrayAllocBase
	{
		//these virtual functions are used for serialization purposes, overrider to call the implementations
		virtual void	VAlloc(size_t size) {}
		virtual void	VFree(size_t size) {}
		virtual void	VRealloc(size_t newSize, size_t preSize) {}
		virtual void*	VData() { return nullptr; }

		//stack allocator class must return its capacity
		constexpr static size_t StackCapacity() { return 0; }
		//the minimum value (number of element) that array grows for allocating new space
		constexpr static size_t IncSize() { return 32; }
	};


	//stack allocator for TArray
	template<typename TElement, unsigned MaxLength> struct TArrayAllocStack : public ArrayAllocBase
	{
		static const size_t BufferSize = sizeof(TElement) * MaxLength;
		char mBuffer[BufferSize];

		virtual void	VAlloc(size_t size)  override { Alloc(size); }
		virtual void	VFree(size_t size) override { Free(size); }
		virtual void	VRealloc(size_t preSize, size_t newSize) override { Realloc(preSize, newSize); }
		virtual void*	VData() override { return Data(); }

		void Alloc(size_t size)
		{
		}
		void Realloc(size_t preSize, size_t newSize)
		{
		}
		void Free(size_t size)
		{
		}
		const void* Data() const { return mBuffer; }
		void* Data() { return mBuffer; }

		constexpr static size_t StackCapacity() { return MaxLength; }
		constexpr static size_t IncSize() { return 1; }
	};

	//default memory allocator for TArray
	template<typename TElement> struct TArrayAllocDefault : public ArrayAllocBase
	{
		void*	mArray = nullptr;

		virtual void	VAlloc(size_t size)  override { Alloc(size); }
		virtual void	VFree(size_t size) override { Free(size); }
		virtual void	VRealloc(size_t preSize, size_t newSize) override { Realloc(preSize, newSize); }
		virtual void*	VData() override { return mArray; }

		void Alloc(size_t size)
		{
			mArray = MemAlloc(size);
		}
		void Realloc(size_t preSize, size_t newSize)
		{
			mArray = MemRealloc(mArray, newSize);
		}
		void Free(size_t size)
		{
			MemFree(mArray);
		}

		void* Data() const { return mArray; }
	};




	template<typename TElement, typename TAllocator = TArrayAllocDefault<TElement>> class TArray
	{
	public:
		using ElementT = TElement;
		using AllocatorT = TAllocator;

		//default constructor
		TArray(TAllocator allocator = TAllocator())
			: mLength(0), mCapacity(TAllocator::StackCapacity()), mAllocator(allocator)
		{

		}
		//default constructor with initial capacity
		TArray(size_t initialCapacity, TAllocator allocator = TAllocator())
			: mLength(0), mCapacity(TAllocator::StackCapacity()), mAllocator(allocator)
		{
			SetCapacity(initialCapacity);
		}
		//constructing from a raw array
		TArray(const TElement* elements, size_t numElement, TAllocator allocator = TAllocator())
			: mLength(0), mCapacity(TAllocator::StackCapacity()), mAllocator(allocator)
		{
			if (elements && numElement > 0)
			{
				mLength = numElement;

				SetCapacity(numElement);
				CallCCTor(Elements(), elements, numElement);
			}
		}
		//constructing from initializer list
		TArray(const std::initializer_list<TElement>& list, TAllocator allocator = TAllocator())
			: TArray(list.begin(), list.size(), allocator)
		{
		}
		//copy constructor
		TArray(const TArray& copy)
			: TArray(copy.Elements(), copy.Length())
		{
		}
		//copy constructor
		template<class TOtherAllocator> TArray(const TArray<TElement, TOtherAllocator>& copy)
			: TArray(copy.Elements(), copy.Length())
		{
		}
		//move constructor
		TArray(TArray&& move)
			: mLength(move.mLength), mCapacity(move.mCapacity), mAllocator(move.mAllocator)
		{
			move.mLength = 0;
			move.mCapacity = 0;
		}

		//move assignment
		TArray& operator = (TArray&& move)
		{
			if(this != &move)
			{
				this->Empty();
				new (this) TArray(move);
			}
			return *this;
		}
		//destructor
		~TArray()
		{
			Empty();
		}
		//copy assignment
		TArray& operator = (const TArray& other)
		{
			Empty();
			new (this) TArray(other);
			return *this;
		}
		//copy assignment
		template<typename TOtherAllocator> TArray& operator = (const TArray<TElement, TOtherAllocator>& other)
		{
			Empty();
			new (this) TArray(other);
			return *this;
		}
		//append to the end of array
		template<typename TOtherAllocator> TArray& operator += (const TArray<TElement, TOtherAllocator>& other)
		{
			Append(other.Elements(), other.Length());
			return *this;
		}
		//append to the end of array
		void Append(const TElement* elements, size_t numElement)
		{
			if (elements && numElement)
			{
				SetCapacity(mLength + numElement);
				CallCCTor(Elements() + mLength, elements, numElement);
				mLength += numElement;
			}
		}
		template<typename TOtherAllocator> void Append(const TArray<TElement, TOtherAllocator>& array)
		{
			Append(array.Elements(), array.Length());
		}
		void Append(const std::initializer_list<TElement>& list)
		{
			Append(list.begin(), list.size());
		}
		void AppendUnique(const TElement* elements, size_t numElement)
		{
			if (elements == nullptr) return;

			for (size_t i = 0; i < numElement; i++)
				AddUnique(elements[i]);
		}
		template<typename TOtherAllocator> void AppendUnique(const TArray<TElement, TOtherAllocator>& array)
		{
			AppendUnique(array.Elements(), array.Length());
		}

		/*
		increase the capacity of array, depends on the TAllocator
		*/
		void IncCapacity(size_t increment = TAllocator::IncSize())
		{
			if (TAllocator::StackCapacity() > 0) //is stack allocator ?
			{
				UASSERT(mCapacity + increment <= TAllocator::StackCapacity());
				return;
			}

			if (mLength)
			{
				mAllocator.Realloc(sizeof(TElement) * mCapacity, sizeof(TElement) * (mCapacity + increment));
				mCapacity += increment;
			}
			else
			{
				mCapacity = increment;
				mAllocator.Alloc(sizeof(TElement) * mCapacity);
			}
		}
		//sets the new capacity. if new capacity is less than the current length of array, destructor will be called on the waste elements
		void SetCapacity(size_t newCapacity)
		{
			if (TAllocator::StackCapacity() > 0) //is stack allocator ?
			{
				UASSERT(newCapacity <= TAllocator::StackCapacity());
				return;
			}

			if (newCapacity)
			{
				auto newLength = Min(mLength, newCapacity);

				if (mLength)
				{
					if (newLength < mLength)
						CallDTor(Elements() + newLength, mLength - newLength);

					mAllocator.Realloc(mCapacity * sizeof(TElement), newCapacity * sizeof(TElement));
					mLength = newLength;
					mCapacity = newCapacity;
				}
				else
				{
					mLength = newLength;
					mAllocator.Alloc(newCapacity * sizeof(TElement));
				}
			}
			else
			{
				Empty();
			}
		}
		void Empty()
		{
			if (mLength)
			{
				CallDTor(Elements(), mLength);
				mLength = 0;

				mAllocator.Free(mCapacity * sizeof(TElement));

				if (TAllocator::StackCapacity() == 0) //is not stack allocator?
					mCapacity = 0;
			}
		}
		TElement& LastElement()
		{
			UASSERT(Elements() && mLength > 0);
			return Elements()[mLength - 1];
		}
		const TElement& LastElement() const
		{
			UASSERT(Elements() && mLength > 0);
			return Elements()[mLength - 1];
		}
		/*
		adds elements to the end of array without any initialization and returns index of the first added element
		*/
		size_t AddUnInit(size_t count = 1)
		{
			if (mCapacity < (mLength + count)) IncCapacity(count);
			auto prelength = mLength;
			mLength += count;
			return prelength;
		}
		/*
		adds elements to the end of array with default constructor initialization and returns index of the first added element
		*/
		size_t AddDefault(size_t count = 1)
		{
			if (mCapacity < (mLength + count)) IncCapacity(count);
			auto preLength = mLength;
			mLength += count;
			CallDCTor(Elements() + preLength, count);
			return preLength;
		}
		//adds an element at the end of array
		template<typename... TArgs> size_t Add(TArgs... args)
		{
			if (mCapacity == mLength) IncCapacity();
			new (Elements() + mLength) TElement(args...);
			return mLength++;
		}
		size_t AddUnique(const TElement& element)
		{
			size_t foundIndex = Find(element);
			if (foundIndex == INVALID_INDEX)
				return Add(element);
			else
				return foundIndex;
		}
		template<typename TLambda, typename TCmpAndAdd> size_t AddUniqueLamda(TLambda compare, TCmpAndAdd element)
		{
			for (size_t i = 0; i < mLength; i++)
			{
				if (compare(Elements()[i], element))
					return i;
			}

			return Add(element);
		}
		//remove an element from the end of array and calls dtor
		void Pop()
		{
			UASSERT(mLength > 0);
			mLength--;
			(Elements() + mLength)->~TElement();
		}
		void PopN(size_t count)
		{
			UASSERT(count <= mLength);
			CallDCTor(Elements() + mLength - count, count);
			mLength -= count;
		}
		void Pop(TElement& out)
		{
			UASSERT(mLength > 0);
			mLength--;
			out = std::move(Elements()[mLength]);
		}
		//remove an element at the specified index and move last element to it
		void RemoveAtSwap(size_t index)
		{
			ArrayRemoveAtSwap(Elements(), mLength, index);
		}
		bool RemoveSwap(const TElement& element)
		{
			size_t index = Find(element);
			if (index != INVALID_INDEX)
			{
				RemoveAtSwap(index);
				return true;
			}
			return false;
		}
		//remove all elements without releasing memory
		void RemoveAll()
		{
			CallDTor(Elements(), mLength);
			mLength = 0;
		}
		void Reverse()
		{
			ArrayReverse(Elements(), mLength);
		}
		operator TArray<const TElement>& () { return *((TArray<const TElement>*)this); }

		//only calls destructor on removing elements
		void RemoveShift(size_t index, size_t count = 1)
		{
			ArrayRemoveShift(Elements(), mLength, index, count);
		}
		template<typename TLambda> void ConditionalRemove(TLambda proc)
		{
			ArrayConditionalRemove(Elements(), mLength, proc);
		}
		template<typename TLambda> void ConditionalRemovePOD(TLambda proc)
		{
			ArrayConditionalRemovePOD(Elements(), mLength, proc);
		}

		void Shrink()
		{
			UASSERT(mCapacity >= mLength);
			SetCapacity(mLength);
		}
		size_t Length() const { return mLength; }
		size_t Capacity() const { return mCapacity; }

		TElement* Elements() const { return (TElement*)mAllocator.Data(); }

		//checks whether @ptr is within array bound or not
		bool IsWithin(const void* ptr) const
		{
			return ((size_t)ptr) >= ((size_t)Elements()) && ((size_t)ptr) < ((size_t)(Elements() + mLength));
		}
		const TElement& operator [] (size_t index) const
		{
			UASSERT(Elements() && index < mLength);
			return Elements()[index];
		}
		TElement& operator [] (size_t index)
		{
			UASSERT(Elements() && index < mLength);
			return Elements()[index];
		}
		bool IsIndexValid(size_t index) const
		{
			return index < mLength;
		}
		size_t Find(const TElement& element) const
		{
			for (size_t i = 0; i < mLength; i++)
				if (element == Elements()[i]) return i;
			return INVALID_INDEX;
		}
		//lambda must return true if element was found
		template<typename Lambda> size_t FindLambda(Lambda compareLambda) const
		{
			for (size_t i = 0; i < mLength; i++)
				if (compareLambda(Elements()[i])) return i;
			return INVALID_INDEX;
		}
		size_t FindLast(const TElement& element) const
		{
			size_t find = INVALID_INDEX;
			for (size_t i = 0; i < mLength; i++)
				if (element == Elements()[i])
					find = i;
			return find;
		}
		template<typename Lambda> size_t FindLastLambda(Lambda compareLambda) const
		{
			size_t find = INVALID_INDEX;
			for (size_t i = 0; i < mLength; i++)
				if (compareLambda(Elements()[i]))
					find = i;
			return find;
		}
		bool HasElement(const TElement& element) const { return Find(element) != INVALID_INDEX; }

		//returns true if content of this array is equal with @other
		template<typename TOtherAllocator> bool IsEqual(const TArray<TElement, TOtherAllocator>& other, bool bUseMemCmp) const
		{
			if (this->mLength == other.mLength) //same length?
			{
				if (this->Elements() == other.Elements())	//same address?
					return true;

				if (bUseMemCmp)
				{
					return MemCmp(this->Elements(), other.Elements(), mLength * sizeof(TElement)) == 0;
				}
				else
				{
					for (size_t iElement = 0; iElement < mLength; iElement++)
					{
						if (this->Elements()[iElement] != other.Elements()[iElement])
							return false;
					}
					return true;
				}
			}
			return false;
		}
		template<typename Lambda> bool HasElementLambda(Lambda compareLambda) const
		{
			for (size_t i = 0; i < mLength; i++)
				if (compareLambda(Elements()[i])) return true;
			return false;
		}

		template<typename TElementTo> TArray<TElementTo>& CastedRef()
		{
			return *((TArray<TElementTo>*)(this));
		}
		template<typename TElementTo> const TArray<TElementTo>& CastedRef() const
		{
			return *((const TArray<TElementTo>*)(this));
		}


		class Iter
		{
		public:
			TElement* ptr;
			Iter(TElement* p) : ptr(p) {}
			bool operator !=(Iter rhs) const { return ptr != rhs.ptr; }
			TElement& operator*() { return *ptr; }
			void operator++() { ptr++; }
		};
		class ConstIter
		{
		public:
			const TElement* ptr;
			ConstIter(const TElement* p) : ptr(p) {}
			bool operator !=(ConstIter rhs) const { return ptr != rhs.ptr; }
			const TElement& operator*() const { return *ptr; }
			void operator++() { ptr++; }
		};

		Iter begin() { return Iter(Elements()); }
		Iter end() { return Iter(Elements() + mLength); }
		ConstIter begin() const { return ConstIter(Elements()); }
		ConstIter end() const { return ConstIter(Elements() + mLength); }
		ConstIter cbegin() const { return begin(); }
		ConstIter cend() const { return cend(); }

	private:

		static void CallDCTor(TElement* elements, size_t numElement)
		{
			for (size_t i = 0; i < numElement; i++)
				new (elements + i) TElement();
		}
		static void CallCCTor(TElement* elements, const TElement* copies, size_t numELement)
		{
			for (size_t i = 0; i < numELement; i++)
				new (elements + i) TElement(copies[i]);
		}
		static void CallDTor(TElement* elements, size_t numElement)
		{
			for (size_t i = 0; i < numElement; i++)
				(elements + i)->~TElement();
		}

		size_t		mLength;
		size_t		mCapacity;
		TAllocator  mAllocator;
	};

};







