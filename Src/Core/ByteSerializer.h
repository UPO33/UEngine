#pragma once

#include "Array.h"
#include "SmartMemory.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class ByteSerializer
	{
		TArray<byte> mData;
		TArray<size_t> mSizeTokensStack;

	public:
		ByteSerializer& Bytes(const void* bytes, size_t count)
		{
			mData.Append((byte*)bytes, count);
			return *this;
		}
		~ByteSerializer()
		{
			UASSERT(mSizeTokensStack.Length() == 0);
		}

		byte* GetData() const { return mData.Elements(); }
		//returns current pos, number of written bytes
		size_t GetCurPos() const { return mData.Length(); }
		void IncCapacity(size_t value) { mData.IncCapacity(value); }

		
		void PushSizeToken()
		{
			//store current pos
			mSizeTokensStack.Add(mData.Length());
			uint32 token = 0;
			*this << token;
		}
		void PopSizeToken()
		{
			size_t tokenIndex = 0;
			mSizeTokensStack.Pop(tokenIndex);
			uint32 tokenSize = GetCurPos() - tokenIndex;
			*((uint32*)(GetData() + tokenIndex)) = tokenSize;
		}
	};

	template<typename T> ByteSerializer& operator << (ByteSerializer& ser, const T& value)
	{
		ser.Bytes(&value, sizeof(value));
		return ser;
	}
	//////////////////////////////////////////////////////////////////////////
	class ByteDeserializer
	{
		TSPtr<SmartMemBlock>	mBuffer;
		size_t					mCurrentPos;
		bool					mHasError;
		

	public:
		ByteDeserializer(const TSPtr<SmartMemBlock>& buffer)
			: mBuffer(buffer), mCurrentPos(0), mHasError(false)
		{}
		~ByteDeserializer()
		{}
		size_t GetCurPos() const { return mCurrentPos; }
		byte* GetCurData() const { return GetData() + mCurrentPos; }
		byte* GetData() const { return (byte*)mBuffer->Memory(); }
		size_t GetBufferSize() const { return mBuffer->Size(); }

		ByteDeserializer& Bytes(void* bytes, size_t count)
		{
			if (count && !mHasError)
			{
				UASSERT(bytes);
				size_t nFree = GetBufferSize() - mCurrentPos;
				if (count <= nFree)
				{
					MemCopy(bytes, GetCurData(), count);
					mCurrentPos += count;
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
			size_t nFree = GetBufferSize() - mCurrentPos;
			if (count > nFree)
				mHasError = true;
			mCurrentPos += count;
			return *this;
		}
		uint32 ReadTokenSize() 
		{
			uint32 token = 0;
			*this >> token;
			return token;
		}
		//reads current pos as a token an returns a new deserializer pointed at the head of token
		ByteDeserializer ReadToken()
		{
			uint32 tokenSize = ReadTokenSize();
			size_t tokenDataHead = GetCurPos();
			IgnoreBytes(tokenSize);
			ByteDeserializer ret(new SmartMemBlock(GetData() + tokenDataHead, tokenSize));
			ret.mHasError = this->HasError();
			return ret;
		}
		void IgnoreToken()
		{
			IgnoreBytes(ReadTokenSize());
		}
		bool HasError() const { return mHasError; }
	};
	template<typename T> ByteDeserializer& operator >> (ByteDeserializer& ser, T& value)
	{
		ser.Bytes(&value, sizeof(T));
		return ser;
	}

};