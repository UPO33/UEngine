#include "Name.h"
#include "Hash.h"
#include "Memory.h"
#include "Meta.h"
#include "Threading.h"
#include "ByteSerializer.h"

namespace UCore
{
	UCLASS_BEGIN_IMPL(Name)
	UCLASS_END_IMPL(Name)
};

namespace UCore
{
	using NameHashClassT = FNV32;

	NameInstance* NameInstance::EmptyInstance()
	{
		static NameInstance SEmptyInstance = { NameHashClassT::GetPrime(), NameHashClassT::GetPrime(), 0, nullptr, 0 };
		return &SEmptyInstance;
	}

	//////////////////////////////////////////////////////////////////////////
	struct NameContext
	{
		static unsigned const TABLE_SIZE = 8192;

		inline static uint32 HashToTableIndex(uint32 hash) { return hash & (TABLE_SIZE - 1); }

		std::atomic<NameInstance*>*	mTable;

		NameContext()
		{
			mTable = new std::atomic<NameInstance*>[TABLE_SIZE];
			MemZero(mTable, sizeof(std::atomic<NameInstance*>) * TABLE_SIZE);
		}
		~NameContext()
		{
			for (size_t iTable = 0; iTable < TABLE_SIZE; iTable++)
			{
				NameInstance* iter = mTable[iTable];
				while (iter)
				{
					auto tmp = iter;
					iter = iter->mNext;

					if (tmp != NameInstance::EmptyInstance())
						MemFree(tmp);
				}
				mTable[iTable] = nullptr;
			}
			
			delete[] mTable;
		}
		//////////////////////////////////////////////////////////////////////////
		NameInstance* Get(const NameCharT* strName, size_t nameLength)
		{
			if (strName == nullptr || strName[0] == 0 || nameLength == 0)
				return NameInstance::EmptyInstance();



			UASSERT(nameLength <= NAME_MAX_LENGTH);

			//to lower
			NameCharT strNameLowerCase[NAME_MAX_LENGTH + 1];
			for (size_t iChar = 0; iChar < nameLength; iChar++)
				strNameLowerCase[iChar] = ::tolower(strName[iChar]);
			strNameLowerCase[nameLength] = 0;

			NameCharT	nameHashLowerCase = NameHashClassT::HashBuffer(strNameLowerCase, nameLength);
			NameHashT	nameHash = NameHashClassT::HashBuffer(strName, nameLength);

			auto tableIndex = HashToTableIndex(nameHash);

			//searching the table
			{
				NameInstance* iter = mTable[tableIndex];

				while (iter)
				{
					if (iter->mHash == nameHash)	//found in table?
					{
						//check hash conflict
						UASSERT(iter->mLength == nameLength);
						UASSERT(MemCmp(iter->CStr(), strName, nameLength * sizeof(NameCharT)) == 0);

						return iter;
					}

					iter = iter->mNext;
				}
			}

			//nothing found, so a create new name
			{
				NameInstance* newInstance = (NameInstance*)MemAlloc(sizeof(NameInstance) + nameLength + 1);

				newInstance->mHash = nameHash;
				newInstance->mHashLowerCase = nameHashLowerCase;
				newInstance->mLength = nameLength;
				newInstance->mNext = mTable[tableIndex];

				//copying string
				MemCopy(&(newInstance->mFirstChar), strName, nameLength * sizeof(NameCharT));

				//null terminating the string
				(&(newInstance->mFirstChar))[nameLength] = 0;

				{
					mTable[tableIndex] = newInstance;
				}

				return newInstance;
			}
		}

		NameInstance* Get(const NameCharT* inName)
		{
			if (inName == nullptr) 
				return NameInstance::EmptyInstance();

			return Get(inName, UStrLen(inName));
		}
		//////////////////////////////////////////////////////////////////////////
		static NameContext& Get()
		{
			static NameContext Sing;
			return Sing;
		}
		//////////////////////////////////////////////////////////////////////////
		void DbgLog()
		{
			for (size_t iTable = 0; iTable < TABLE_SIZE; iTable++)
			{
				NameInstance* iter = mTable[iTable];
				while (iter)
				{
					ULOG_MESSAGE("Address: [%],  Value:[%],  Hash:[%]", (size_t)iter, iter->CStr(), iter->mHash);
					iter = iter->mNext;
				}
			}
		}
	};

	NameInstance* NameInstance::Insert(const NameCharT* nullTerminatedStr)
	{
		return NameContext::Get().Get(nullTerminatedStr);
	}

	NameInstance* NameInstance::Insert(const NameCharT* str, size_t strLength)
	{
		return NameContext::Get().Get(str, strLength);
	}


	void Name::MetaSerialize(ByteSerializer& ser)
	{
		UASSERT(NAME_MAX_LENGTH < 0xFFff);
		UASSERT(Length() <= NAME_MAX_LENGTH);

		uint16 nameLength = (uint16)Length();
		ser << nameLength;
		if (nameLength)
			ser.Bytes((char*)CStr(), nameLength);
	}

	void Name::MetaDeserialize(ByteDeserializer& ser)
	{
		uint16 nameLength = 0;
		ser >> nameLength;
	
		if (nameLength)
		{
			UASSERT(nameLength <= NAME_MAX_LENGTH);

			*this = Name((NameCharT*)ser.GetCurData(), nameLength);
			ser.IgnoreBytes(nameLength * sizeof(NameCharT));
		}
		else
		{
			//not required, is supposed default constructed
			//*this = Name();
		}
	}

	UCORE_API void ZZDdbgLogNameTable()
	{
		NameContext::Get().DbgLog();
	}

};