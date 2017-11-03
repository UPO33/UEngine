#include "Hash.h"

#ifdef UPLATFORM_ANDROID
#include <zlib.h>
#define USE_ZLIB_CRC
#else
#ifdef UCORE_HAS_ZLIB
#include <zlib/zlib.h>
#define USE_ZLIB_CRC
#endif
#endif


namespace UCore
{

#ifdef USE_ZLIB_CRC
	uint32 CRC32::GetPrime()
	{
		return crc32(0L, Z_NULL, 0);
	}

	uint32 CRC32::HashBuffer(const void* buffer, size_t length, uint32 hash)
	{
		return crc32(hash, (const Byte*)buffer, length);
	}
#else
	uint32 CRC32::GetPrime()
	{
		float ss, cc;
		SinCos(0, ss, cc);
		UASSERT(false);
		return 0;
	}

	uint32 CRC32::HashBuffer(const void* buffer, size_t length, uint32 hash)
	{
		UASSERT(false);
		return 0;
	}
#endif

	uint32 CRC32::HashStr(const char* str, uint32 hash /*= GetPrime()*/)
	{
		return HashBuffer(str, UStrLen(str), hash);
	}

	uint32 FNV32::HashBuffer(const void* buffer, size_t length, uint32 hash)
	{
		UASSERT(buffer);
		unsigned char* p = (unsigned  char*)buffer;
		for (size_t i = 0; i < length; i++)
		{
			hash = (hash * 16777619) ^ p[i];
		}
		return hash;
	}

	uint32 FNV32::HashStr(const char* str, uint32 hash /*= GetPrime()*/)
	{
		return HashBuffer(str, UStrLen(str), hash);
	}

	uint64 FNV64::HashBuffer(const void* buffer, size_t length, uint64 hash)
	{
		UASSERT(buffer);
		unsigned char* p = (unsigned char*)buffer;
		for (size_t i = 0; i < length; i++)
		{
			hash = (hash * 1099511628211ULL) ^ p[i];
		}
		return hash;
	}

	uint64 FNV64::HashStr(const char* str, uint64 hash)
	{
		return HashBuffer(str, UStrLen(str), hash);
	}

}
