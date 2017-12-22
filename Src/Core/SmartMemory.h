#pragma once

#include "SmartPointers.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class SmartMemBlock : public ISmartBase
	{
	public:
		//null initialize
		SmartMemBlock() : mMemory(nullptr), mSize(0)
		{}
		//allocate and zero initialize
		SmartMemBlock(size_t size) : mSize(size), mMemory(nullptr)
		{
			if (size) mMemory = MemAllocZero(size);
		}
		//@pMemory memory to copy data from
		//@size size of the memory in bytes
		SmartMemBlock(void* pMemory, size_t size)
		{
			if (pMemory && size)
			{
				mSize = size;
				mMemory = MemAlloc(size);
				MemCopy(mMemory, pMemory, size);
			}
			else
			{
				mMemory = nullptr;
				mSize = 0;
			}
		}
		~SmartMemBlock()
		{
			MemFreeSafe(mMemory);
			mSize = 0;
		}
		SmartMemBlock(const SmartMemBlock& copy)
		{
			mSize = copy.mSize;
			mMemory = nullptr;

			if (copy.mSize)
			{
				UASSERT(copy.mMemory);
				mMemory = MemAlloc(copy.mSize);
				UASSERT(mMemory);
				MemCopy(mMemory, copy.mMemory, copy.mSize);
			}
		}
		SmartMemBlock(SmartMemBlock&& move)
		{
			mMemory = move.mMemory;
			mSize = move.mSize;

			move.mMemory = nullptr;
			move.mSize = 0;
			move.~SmartMemBlock();
		}
		SmartMemBlock& operator = (const SmartMemBlock& copy)
		{
			this->~SmartMemBlock();
			new (this) SmartMemBlock(copy);
			return *this;
		}
		SmartMemBlock& operator = (SmartMemBlock&& move)
		{
			this->~SmartMemBlock();
			new (this) SmartMemBlock(move);
			return *this;
		}
		size_t Size() const { return mSize; }
		void*  Memory() const { return mMemory; }
		bool IsNull() const { return mMemory == nullptr; }

	private:
		void*	mMemory;
		size_t	mSize;
	};
};
