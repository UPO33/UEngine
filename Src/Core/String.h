#pragma once

#include "Base.h"
#include "Memory.h"
#include "Array.h"

#include <string.h>


namespace UCore
{
#if UPLATFORM_WIN
	static const char PATH_SEPARATOR_CHAR = '\\';
#else
	static const char PATH_SEPARATOR_CHAR = '/';
#endif


	class Name;


	//////////////////////////////////////////////////////////////////////////
	inline size_t UStrLen(const char* str) { return strlen(str); }
	
	inline bool UStrHasPrefix(const char* str, const char* begin) { return strstr(str, begin) == str; }
	UCORE_API bool UStrHasSuffix(const std::string& str, const std::string& suffix);

	inline void StrCopy(char* dst, const char* src) {  strcpy(dst, src); }
	UCORE_API bool StrTakeBetweenFirstLast(const char* str, char* out, char first, char last);
	UCORE_API size_t StrCountChar(const char* str, char chr);
	UCORE_API const char* StrFindNChar(const char* str, char chr, unsigned n);
	UCORE_API const char* StrFindRNChar(const char* str, char chr, unsigned n);

	template<typename TChar> struct TStrRange
	{
		const TChar* mBegin = nullptr;
		const TChar* mEnd = nullptr;

		bool IsNull() const { return mBegin == nullptr; }
		bool IsEmpty() const { return mBegin == mEnd; }
		size_t Length() const { return (((size_t)mEnd) - ((size_t)mBegin)) / sizeof(TChar); }
		bool IsEqual(const TChar* str) const
		{
		}
	};
	template<typename TChar> StringStreamOut& operator << (StringStreamOut& s, TStrRange<TChar> strRange)
	{
		for (size_t iChar = 0; iChar < strRange.Length(); iChar++)
		{
			s << strRange.mBegin[iChar];
		}
		return s;
	}
	UCORE_API const char* UStrStr(const char* str, TStrRange<char> subStr, bool caseSensitive = true);
	UCORE_API const wchar_t* UStrStr(const wchar_t* str, TStrRange<wchar_t> subStr, bool caseSensitive = true);
	
	//////////////////////////////////////////////////////////////////////////
	template<typename TChar, typename TAlloc> void USplitString(const TChar* inStr, const TChar* inSeprators, TArray<TStrRange<TChar>, TAlloc>& outRanges)
	{		
		if (inStr == nullptr || inSeprators == nullptr) return;
		
		size_t strLength = UStrLen(inStr);
		size_t sepLength = UStrLen(inSeprators);
		if (strLength == 0 || sepLength == 0) return;

		const TChar* wb = inStr;
		const TChar* we = wb;
		const TChar* e = inStr + strLength;

		auto LHasSeprator = [](TChar chr, const TChar* seps, size_t numSeps){
			for (size_t i = 0; i < numSeps; i++)
				if (chr == seps[i])
					return true;
			return false;
		};

		while (we < e)
		{
			if (LHasSeprator(*we, inSeprators, sepLength))
			{
				outRanges.Add(TStrRange<TChar> {wb, we });
				wb = we + 1;
			}
			we++;
		}
		if (wb != we)
			outRanges.Add(TStrRange<TChar>{wb, we });
	}

	class UCORE_API String
	{
		UCLASS(String)

		using CharT = char16_t;

		String() {}
		String(const CharT* nullTerminatedStr): mStr(nullTerminatedStr) {}
		String(const CharT* str, size_t length) : mStr(str, length) {}
		explicit String(const std::u16string& stdStr) : mStr(stdStr) {}

		size_t Length() const { return mStr.length(); }
		size_t Capacity() const { return mStr.capacity(); }

		void Empty() { mStr.clear(); }
		bool IsEmpty() const { return mStr.empty(); }
		
		operator bool() const { return IsEmpty(); }

		CharT operator[] (size_t index) const 
		{
			UASSERT(index < Length());
			return mStr[index];
		}
		CharT& operator[] (size_t index)
		{
			UASSERT(index < Length());
			return mStr[index];
		}
		
		const CharT* GetCharacters() const { return mStr.c_str(); }
		
		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);

		std::u16string mStr;
	};
};
