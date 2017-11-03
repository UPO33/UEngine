#include "Compression.h"
#include "Memory.h"

#ifdef UPLATFORM_ANDROID
//android has zlib it self
#include <zlib.h>
#define USE_ZLIB_COMPRESSION
#else
#ifdef UCORE_HAS_ZLIB
//include zlib from third party
#include <zlib/zlib.h>
#define USE_ZLIB_COMPRESSION
#endif
#endif

namespace UCore
{
#ifdef USE_ZLIB_COMPRESSION
	UCORE_API void* UCompressBuffer(const void* inBuffer, size_t inBufferSize, size_t& outBufferSize)
	{
		UASSERT(inBuffer && inBufferSize);

		outBufferSize = 0;

		auto dstSize = ::compressBound((uLong)inBufferSize);
		Bytef* dst = (Bytef*)MemAlloc(dstSize + sizeof(uint32));
		*((uint32*)dst) = inBufferSize;

		if (::compress(dst + sizeof(uint32), &dstSize, (const Bytef*)inBuffer, (uLong)inBufferSize) == Z_OK)
		{
			outBufferSize = (size_t)dstSize + sizeof(uint32);
			return dst;
		}
		else
		{
			MemFree(dst);
			return nullptr;
		}
	}

	UCORE_API void* UDeCompressBuffer(const void* inBuffer, size_t inBufferSize, size_t& outBufferSize)
	{
		UASSERT(inBuffer && inBufferSize);

		outBufferSize = 0;

		uLongf uncompressedSize = *((uint32*)inBuffer);

		if (uncompressedSize == 0) return nullptr;

		void* dst = MemAlloc(uncompressedSize);
		if (::uncompress((Bytef*)dst, &uncompressedSize, ((const Bytef*)inBuffer) + sizeof(uint32), (uLong)(inBufferSize - sizeof(uint32))) == Z_OK)
		{
			outBufferSize = (size_t)uncompressedSize;
			return dst;
		}
		else
		{
			MemFree(dst);
			return nullptr;
		}
	}
#else
	UCORE_API void* UCompressBuffer(const void* inBuffer, size_t inBufferSize, size_t& outBufferSize)
	{
		ULOG_FATAL("");
		return nullptr;
	}
	UCORE_API void* UDeCompressBuffer(const void* inBuffer, size_t inBufferSize, size_t& outBufferSize)
	{
		ULOG_FATAL("");
		return nullptr;
	}

#endif
};