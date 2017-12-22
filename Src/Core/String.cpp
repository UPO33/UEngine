#include "String.h"
#include "Name.h"
#include "Meta.h"
#include "ByteSerializer.h"

#include <string>
#include <sstream>
#include <stdarg.h>

namespace UCore
{
	UCLASS_BEGIN_IMPL(String)
	UCLASS_END_IMPL(String)

	

};

namespace UCore
{

	bool UStrHasSuffix(const char* str, const char* suffix, bool caseSensitive)
	{
		UFATAL(0);
		auto _strLen = strlen(str);
		auto _suffixLen = strlen(suffix);

		if (_strLen >= _suffixLen)
		{
		}
		return false;
	}

	UCORE_API bool UStrHasSuffix(const std::string& str, const std::string& suffix)
	{
		return str.size() >= suffix.size() &&
			str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	//////////////////////////////////////////////////////////////////////////
	bool StrTakeBetweenFirstLast(const char* str, char* out, char first, char last)
	{
		UASSERT(str && out);
		const char* iter = str;
		const char* firstFound = nullptr;
		const char* lastFound = nullptr;

		while (*iter)
		{
			if (*iter == first && firstFound == nullptr)
				firstFound = iter;
			if (*iter == last)
				lastFound = iter;
		}
		if (firstFound < lastFound)
		{
			unsigned iter = 1;

			while (firstFound + iter != lastFound)
			{
				out[iter] = firstFound[iter];
			}
			out[iter] = 0;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t StrCountChar(const char* str, char chr)
	{
		UASSERT(str);
		size_t n = 0;
		while (*str)
		{
			if (*str == chr) n++;
			str++;
		}
		return n;
	}
	//////////////////////////////////////////////////////////////////////////
	const char* StrFindNChar(const char* str, char chr, unsigned n)
	{
		UASSERT(str);
		while (*str)
		{
			if (*str == chr)
			{
				if (n == 0) return str;
				n--;
			}
			str++;
		}
		return nullptr;
	}
	const char* StrFindRNChar(const char* str, char chr, unsigned n)
	{
		UASSERT(str);
		auto len = UStrLen(str);
		for (int i = len - 1; i >= 0; i--)
		{
			if (str[i] == chr)
			{
				if (n == 0) return str + i;
				n--;
			}
		}
		return nullptr;
	}

// 	template<typename TChar> const TChar* ZZStrStr(const TChar* str, TStrRange<TChar> subStr)
// 	{
// 		if (subStr.Length() == 0) return nullptr;
// 
// 		while (TChar chr = *str)
// 		{
// 			if (MemCmp(str, subStr.mBegin, subStr.Length()) == 0)
// 				return str;
// 			str++;
// 		}
// 		return nullptr;
// 	}

	
	template<typename TChar, bool bCaseSensitive> const TChar* ZZStrStr(const TChar* str, TStrRange<TChar> subStr)
	{
		if (subStr.Length() == 0) return nullptr;

		auto LTestStrEqual = [](const TChar* str0, const TChar* str1, size_t str1Length)
		{
			for (size_t i = 0; i < str1Length; i++)
			{
				if (bCaseSensitive)
				{
					if (str0[i] != str1[i])
						return false;
				}
				else
				{
					if (tolower(str0[i]) != tolower(str1[i]))
						return false;
				}

			}
			return true;
		};

		while (TChar chr = *str)
		{
			if (LTestStrEqual(str, subStr.mBegin, subStr.Length()))
				return str;

			str++;
		}
		return nullptr;
	}
	UCORE_API const char* UStrStr(const char* str, TStrRange<char> subStr, bool caseSensitive)
	{
		if(caseSensitive)
			return ZZStrStr<char, true>(str, subStr);
		else
			return ZZStrStr<char, false>(str, subStr);
	}

	UCORE_API const wchar_t* UStrStr(const wchar_t* str, TStrRange<wchar_t> subStr, bool caseSensitive)
	{
		if(caseSensitive)
			return ZZStrStr<wchar_t, true>(str, subStr);
		else
			return ZZStrStr<wchar_t, false>(str, subStr);

	}


	void String::MetaSerialize(ByteSerializer& ser)
	{
		uint32 len = (uint32)Length();
		ser << len;
		ser.Bytes(GetCharacters(), Length() * sizeof(CharT));
	}

	void String::MetaDeserialize(ByteDeserializer& ser)
	{
		uint32 len = 0;
		len >> len;
		if (len)
		{
			*this = String((const CharT*)ser.GetCurData(), len);
			ser.IgnoreBytes(len);
		}
	}
};