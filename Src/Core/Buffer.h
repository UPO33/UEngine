#pragma once

#include "Base.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class UCORE_API Buffer
	{
	public:
		Buffer() : mBuffer(nullptr), mSize(0) {}
		Buffer(size_t size, void* initialData = nullptr);
		Buffer(const Buffer& copy);
		Buffer& operator = (const Buffer& other);
		~Buffer();
		void Free();
		void* Data() const { return mBuffer; }
		size_t Size() const { return mSize; }

		

	private:
		size_t	mSize;
		void*	mBuffer;
	};


};