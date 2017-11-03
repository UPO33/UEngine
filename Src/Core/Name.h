#pragma once

#include "String.h"

namespace UCore
{
	typedef uint32	NameHashT;
	typedef char	NameCharT;
	static constexpr unsigned NAME_MAX_LENGTH = 1024;

	struct UCORE_API NameInstance
	{
		NameHashT			mHash;
		NameHashT			mHashLowerCase;
		size_t				mLength;
		NameInstance*		mNext;
		NameCharT			mFirstChar;
		

		//return pointer to the empty instance
		static NameInstance* EmptyInstance();
		//get a new instance. if currently exists will return it, otherwise creates a new instance
		static NameInstance* Insert(const NameCharT* nullTerminatedStr);
		static NameInstance* Insert(const NameCharT* str, size_t strLength);
		
		bool IsEmpty() const { return mLength == 0; }
		const NameCharT* CStr() const { return &mFirstChar; }

	};

	UCORE_API void ZZDdbgLogNameTable();

	//////////////////////////////////////////////////////////////////////////incomplete
	class UCORE_API Name
	{
		UCLASS(Name)

		typedef NameHashT HashT;
		typedef NameCharT CharT;

		Name()
		{
			mInstance = NameInstance::EmptyInstance();
		}
		Name(std::nullptr_t)
		{
			mInstance = NameInstance::EmptyInstance();
		}
		Name(const Name& copy)
		{
			mInstance = copy.mInstance;
		}
		~Name()
		{
		}
		Name(const CharT* str)
		{
			mInstance = NameInstance::Insert(str);
		}
		Name(const CharT* str, size_t strLen)
		{
			mInstance = NameInstance::Insert(str, strLen);
		}
		bool IsEmpty() const { return mInstance->IsEmpty(); }
		size_t Length() const { return mInstance->mLength; }
		NameHashT GetHash() const { return mInstance->mHash; }
		NameCharT GetHashLowerCase() const { return mInstance->mHashLowerCase; }
		const NameCharT* CStr() const { return mInstance->CStr(); }
		const NameInstance* GetInstance() const { return mInstance; }

		operator const CharT*() const { return mInstance->CStr(); }

		bool Equal(Name other, bool caseSensitive = true) const
		{
			if (caseSensitive)
				return mInstance->mHash == other.mInstance->mHash;
			else
				return mInstance->mHashLowerCase == other.mInstance->mHashLowerCase;
		}
		bool operator == (Name other) const 
		{ 
			if (mInstance == other.mInstance)
			{
				UASSERT(mInstance->mHash == other.mInstance->mHash);
				return true;
			}
			else
			{
				UASSERT(mInstance->mHash != other.mInstance->mHash);
				return false;
			}
		}
		bool operator != (Name other) const
		{
			return !this->operator ==(other);
		}

		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);

	private:
		NameInstance*	mInstance;
	};

	

	inline uint32 GetHash32(Name name) { return (uint32)name.GetHash(); }
};