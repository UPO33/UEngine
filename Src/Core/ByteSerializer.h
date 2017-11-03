#pragma once

#include "Array.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class ByteSerializer
	{
		TArray<byte> mData;
	public:
		ByteSerializer& Bytes(const void* bytes, size_t count)
		{
			mData.Append((byte*)bytes, count);
			return *this;
		}
		byte* GetData() const { return mData.Elements(); }
		size_t GetSize() const { return mData.Length(); }
		void IncCapacity(size_t value) { mData.IncCapacity(value); }
	};

	template<typename T> ByteSerializer& operator << (ByteSerializer& ser, const T& value)
	{
		ser.Bytes(&value, sizeof(value));
		return ser;
	}
	//////////////////////////////////////////////////////////////////////////
	class ByteDeserializer
	{
		byte*	mBuffer;
		size_t	mSize;
		size_t	mPos;
		bool	mHasError;

	public:
		ByteDeserializer(void* buffer, size_t bufferSize) 
			: mBuffer((byte*)buffer), mSize(bufferSize), mPos(0), mHasError(false)
		{}
		~ByteDeserializer()
		{}
		size_t CurSize() const { return mPos; }
		void* CurData() const { return mBuffer + mPos; }

		ByteDeserializer& Bytes(void* bytes, size_t count)
		{
			if (count && !mHasError)
			{
				UASSERT(bytes);
				size_t nFree = mSize - mPos;
				if (count <= nFree)
				{
					MemCopy(bytes, mBuffer + mPos, count);
					mPos += count;
				}
				else
				{
					mHasError = true;
				}
			}

			return *this;
		}
		ByteDeserializer& IgnoreBytes(size_t count)
		{
			size_t nFree = mSize - mPos;
			if (count > nFree)
				mHasError = true;
			mPos += count;
			return *this;
		}
		bool HasError() const { return mHasError; }
	};
	template<typename T> ByteDeserializer& operator >> (ByteDeserializer& ser, T& value)
	{
		ser.Bytes(&value, sizeof(T));
		return ser;
	}

};